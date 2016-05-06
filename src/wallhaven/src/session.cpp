//
//  session.cpp
//  FraSaver
//
//  Created by franck on 24/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#include "internals.h"
#include "curl.h"
#include <regex>
#include <random>
#include <sstream>

#define WH_HTTP_ROOT "http://alpha.wallhaven.cc"
#define WH_HTTP_LOGIN WH_HTTP_ROOT "/auth/login"
#define WH_HTTP_WALL WH_HTTP_ROOT "/wallpaper"
#define WH_HTTP_SEARCH WH_HTTP_ROOT "/search"
#define WH_HTTP_USER "http://alpha.wallhaven.cc/user"

enum
{
	HTTP_CONTINUE = 100
,	HTTP_SWITCHING_PROTOCOL
,	HTTP_PROCESSING
	
,	HTTP_OK = 200
,	HTTP_CREATED
,	HTTP_ACCEPTED

,	HTTP_MULTIPLE_CHOICE = 300
,	HTTP_MOVED_PERMANENTLY
,	HTTP_FOUND

,	HTTP_BAD_REQUEST = 400
,	HTTP_UNAUTHORIZED
,	HTTP_PAYMENT_REQUIRED
,	HTTP_FORBIDDEN
,	HTTP_NOT_FOUND
,	HTTP_NOT_ALLOWED

};

namespace wallhaven {

	static std::default_random_engine             _rdGen((unsigned int)time(0));
	static std::uniform_real_distribution<double> _rdDis(0.0, 1.0);
	double rnd() { return _rdDis(_rdGen); }

	
	// url=([^"]*)
	
	static std::string regexOneFirstGroup(const std::string & rExp, const std::string & str )
	{
		std::regex r( rExp );
		
		for(auto i  = std::sregex_iterator(str.begin(), str.end(), r); i != std::sregex_iterator(); ++i)
		{
			std::smatch m = *i;
			if (m.size() >= 2)
				return m[1].str();
		}
		return "";
	}
	
	static std::list<std::string> regexAllFirstGroups(const std::string & rExp, const std::string & str )
	{
		std::list<std::string> list;
		std::regex r( rExp );
		
		for(auto i  = std::sregex_iterator(str.begin(), str.end(), r); i != std::sregex_iterator(); ++i)
		{
			std::smatch m = *i;
			if (m.size() >= 2)
				list.push_back(m[1].str());
		}
		return list;
	}

	static std::string urlStem(const std::string & url)
	{
		std::string::size_type i = url.rfind('/');
		if (i == std::string::npos)
			return "";
		
		return url.substr( i + 1 );
	}
	
	static std::string userNameFromUrl(const std::string & userUrl)
	{
		return urlStem( userUrl );
	}
	
	static uint32_t getPageCount( const std::string & src )
	{
		// regex pour page courante / nbPage
		// thumb-listing-page-num"\s*>([0-9]*)[^0-9]*([0-9]*)
		
		const std::string expression = wallhaven::strGetTrim( R"exp(
															 thumb-listing-page-num"\s*>[0-9]*[^0-9]*([0-9]*)
															 )exp" );
		
		const std::string sNum = regexOneFirstGroup(expression, src);
		if (sNum.empty())
			return 0;
		
		std::istringstream iss(sNum);
		uint32_t value;
		iss >> value;
		if (iss.fail() )
			return 0;
		
		return value;
	}

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CSession::CImpl
	{
	public:
		CImpl();
		~CImpl();

	public:
		bool login(const std::string & user, const std::string & pass);
		void logout();
		bool logged() const { return _logged; }
	
	public:
		std::string userUrl() const { return _logged ? userUrl(_userName) : ""; }
		std::string userUrl(const std::string & userName) const { return WH_HTTP_USER "/" + userName; }
		std::string userName() const { return _userName; }
		
		std::map<std::string, std::string> userFavorites(); // [id] = name
		std::map<std::string, std::string> userFavorites(const std::string & userName); // [id] = name
		
	public:
		std::list<std::string> getRandomIds( const std::string & url);
		std::list<std::string> getRandomIds( const CRandomRequestSettings & s);
		std::list<std::string> getRandomIds( const CCollectionRequestSettings & s );
		std::string getFullImageUrl(const std::string & imageId);
		bool updatePageCount(CCollectionRequestSettings & s);

	private:
		CCurl       _c;
		bool        _logged;
		std::string _userName;
	};

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CSession::CImpl::CImpl()
	:	_logged(false)
	{
	}
	
	CSession::CImpl::~CImpl()
	{
	}
	
	static std::string getToken(const std::string & str)
	{
		const std::string expression = strGetTrim( R"exp(
			<[\s]*input[^>]+name[\s]*=[\s]*"_token"[^><]+value[\s]*=[\s]*"([^"]+)"
		)exp" );
		
		return regexOneFirstGroup(expression, str);
	}
	
	void CSession::CImpl::logout()
	{
		_logged = false;
		_userName.clear();
		_c.setopt(CURLOPT_COOKIELIST, "ALL"); // erase all cookies <=> logout
	}



	bool CSession::CImpl::login(const std::string & user, const std::string & pass)
	{
		logout();
		std::string response;
		
		// 1. get token
		
		_c.setopt(CURLOPT_URL, WH_HTTP_LOGIN);
		_c.setopt(CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */
		CURLcode res = _c.perform(response);
		if (res != CURLE_OK)
			return false;
		
		if (_c.getHttpResponseCode() != HTTP_OK)
			return false;
		
		std::string token = getToken(response);
		if (token.empty())
			return false;
		
		// 2. send login/password
		
		const std::string postData = "_token=" + token + "&username=" + user + "&password=" + pass;
		_c.setopt(CURLOPT_POSTFIELDSIZE, postData.length());
		_c.setopt(CURLOPT_POSTFIELDS   , postData.c_str());
		_c.setopt(CURLOPT_POST, 1L);
		res = _c.perform(response);
		_c.setopt(CURLOPT_HTTPGET , 1L);
		if (res != CURLE_OK)
			return false;
		
		if (_c.getHttpResponseCode() != HTTP_FOUND)
			return false;
	
		const std::string userUrl = regexOneFirstGroup("url=([^\"]*)", strNoNewline(response) );
		if (userUrl.empty())
			return false;
	
		_userName = userNameFromUrl(userUrl);
		if (_userName.empty())
			return false;
		
		_logged = true;
		return _logged;
	}
	
	std::map<std::string, std::string> CSession::CImpl::userFavorites(const std::string & userName) // [id] = name
	{
		std::map<std::string, std::string> result;
		
		std::string response;
		_c.setopt(CURLOPT_FOLLOWLOCATION , 1L);
		_c.setopt(CURLOPT_URL, userUrl(userName) + "/favorites");
		CURLcode res = _c.perform(response);
		if (res != CURLE_OK)
			return result;
		
		if (_c.getHttpResponseCode() != HTTP_OK)
			return result;

		const std::string expression = strGetTrim( R"exp(
			<li[^>]*collection[^>]*>(.*?)<\/li>
		)exp" );

		const std::list<std::string> htmlCollectionList = regexAllFirstGroups(expression, strNoNewline(response));
		for (auto col : htmlCollectionList)
		{
			const std::string colUrlExpression = strGetTrim( R"exp(
				href[\s]*=[\s]*"[\s]*(.*?)"
			)exp" );
			const std::string colLabelExpression = strGetTrim( R"exp(
				collection-label[^>]*>(.*?)<
			)exp" );
		
			const std::string url  = regexOneFirstGroup(colUrlExpression, col);
			const std::string label= regexOneFirstGroup(colLabelExpression, col);
			std::string colId = urlStem(url);
			if (!colId.empty())
				result[colId] = label;
		}
	
		return result;
	}
	
	std::map<std::string, std::string> CSession::CImpl::userFavorites() // [id] = name
	{
		if (!logged())
			return std::map<std::string, std::string>();

		return userFavorites(_userName);
	}
	
	
	static void parseIds( std::list<std::string> & ids, const std::string & src)
	{
		const std::string expression = strGetTrim( R"exp(
			data-wallpaper-id[\s]*=[\s]*"([^\"]+)"
		)exp" );
		
		for (auto i : regexAllFirstGroups(expression, src))
			ids.push_back(i);
	}

	std::list<std::string> CSession::CImpl::getRandomIds( const std::string & url)
	{
		std::list<std::string> result;
		_c.setopt(CURLOPT_VERBOSE, 0L);
		_c.setopt(CURLOPT_URL, url.c_str());
		_c.setopt(CURLOPT_HTTPGET , 1L);
		_c.setopt(CURLOPT_FOLLOWLOCATION , 1L);
		
		std::string response;
		const CURLcode res = _c.perform(response);
		
		if (res != CURLE_OK)
			return result;
		
		if (_c.getHttpResponseCode() != HTTP_OK)
			return result;
		
		parseIds( result, response);
		return result;
	}

	std::list<std::string> CSession::CImpl::getRandomIds( const CRandomRequestSettings & s)
	{
		std::vector<std::string> params;
		if (!s.search().empty())
			params.push_back("q=" + _c.escapeString(s.search()));
		
		if (s.categories().any())
			params.push_back( "categories=" + s.categories().to_string() );
		
		if (s.purities().any())
			params.push_back( "purity=" + s.purities().to_string() );

		params.push_back("sorting=random");
		params.push_back("order=desc");

		std::string url = WH_HTTP_SEARCH + std::string("?");
		for (std::size_t i = 0; i< params.size(); ++i)
		{
			url += params[i];
			if (i != params.size() - 1)
				url += "&";
		}
		
		return getRandomIds(url);
	}
	
	static std::string getUrl(const CCollectionRequestSettings & s)
	{
		return WH_HTTP_USER "/" + s.userName() + "/favorites/" + s.colId();
	}

	bool CSession::CImpl::updatePageCount(CCollectionRequestSettings & s)
	{
		if (s.pageCount() >=0 )
			return true;
		
		_c.setopt(CURLOPT_URL, getUrl(s));
		_c.setopt(CURLOPT_HTTPGET , 1L);
		_c.setopt(CURLOPT_FOLLOWLOCATION , 1L);

		std::string src;
		const CURLcode res= _c.perform(src);
		if (res != CURLE_OK)
			return false;
		
		if (_c.getHttpResponseCode() != HTTP_OK)
			return false;

		s.setPageCount( getPageCount(src) );
		return true;
	}

	std::list<std::string> CSession::CImpl::getRandomIds( const CCollectionRequestSettings & s )
	{
		std::string url = getUrl( s );
		if (s.pageCount() > 1 ) {
			const int page = 1 + ((int)(500. * rnd()))%(s.pageCount() - 1);
			url += "?page=" + std::to_string(page);
		}

		const std::list<std::string> a = getRandomIds(url);
		const std::list<std::string> b = shuffle_copy(a);
		return b;
	}

	std::string CSession::CImpl::getFullImageUrl(const std::string & imageId)
	{
		std::string src;
		_c.setopt(CURLOPT_VERBOSE, 0L);
		_c.setopt(CURLOPT_URL, ( WH_HTTP_WALL "/" + imageId).c_str());
		_c.setopt(CURLOPT_HTTPGET , 1L);
		_c.setopt(CURLOPT_FOLLOWLOCATION , 1L);
		const CURLcode res= _c.perform(src);
		if (res != CURLE_OK)
			return "";
		
		if (_c.getHttpResponseCode() != HTTP_OK)
			return "";

		const std::string expression = strGetTrim( R"exp(
			"(//wallpapers.wallhaven.cc/wallpapers/full/wallhaven-[^\"]+)"
		)exp" );
		
		const std::string url= regexOneFirstGroup(expression, src);
		if (url.empty())
			return "";

		return "http:" + url;
	}
	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CSession::CSession()
	:	_p( new CImpl() )
	{
	}

	CSession::~CSession()
	{
	}

	bool CSession::login( const std::string & username, const std::string & password) {
		return _p->login(username, password); }
	
	void CSession::logout() {
		_p->logout(); }

	bool CSession::logged() const {
		return _p->logged(); }

	std::string CSession::userUrl() const {
		return _p->userUrl(); }

	std::string CSession::userUrl(const std::string & userName) const {
		return _p->userUrl(userName); }
	
	std::string CSession::userName() const {
		return _p->userName(); }
	
	std::map<std::string, std::string> CSession::userFavorites() const {
		return _p->userFavorites(); }

	std::map<std::string, std::string> CSession::userFavorites(const std::string & userName) const {
		return _p->userFavorites(userName); }
	
	std::string CSession::getFullImageUrl(const std::string & imageId) const {
		return _p->getFullImageUrl(imageId); }
	
	std::list<std::string> CSession::getRandomIds( const CRandomRequestSettings & s ) const {
		return _p->getRandomIds(s); }

	std::list<std::string> CSession::getRandomIds( const CCollectionRequestSettings & s ) const {
		return _p->getRandomIds(s); }
	
	bool CSession::updatePageCount(CCollectionRequestSettings & s) const {
		return _p->updatePageCount(s); }

}