//
//  wallheavenapi.hpp
//  FraSaver
//
//  Created by franck on 24/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#pragma once
#include <string>
#include <list>
#include <bitset>
#include <map>
#include <vector>

namespace wallhaven
{
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class ECategory : unsigned char { General=2, Anime=1, People=0 };
	enum class EPurity : unsigned char { SFW=2, Sketchy=1, NSFW=0 };

	constexpr std::size_t ECategoryCount(3);
	constexpr std::size_t EPurityCount(3);

	typedef std::bitset<ECategoryCount> CCategories;
	typedef	std::bitset<EPurityCount  > CPurities;

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CRandomRequestSettings
	{
	public:
		CRandomRequestSettings();
		CRandomRequestSettings(const CRandomRequestSettings & src);
		CRandomRequestSettings & operator=(const CRandomRequestSettings & src);
	
	public:
		std::string search() const { return _q; }
		void set( const std::string & s ) { _q = s; }

		const CCategories & categories() const { return _c; }
		CCategories & categories() { return _c; }
		void set( ECategory c, bool bSet=true ) { _c.set(static_cast<int>(c), bSet); }
		bool get( ECategory c ) { return _c[static_cast<int>(c)]; }
		
		const CPurities & purities() const { return _p; }
		CPurities & purities() { return _p; }
		void set( EPurity   p, bool bSet=true ) { _p.set(static_cast<int>(p), bSet); }
		bool get( EPurity   p ) { return _p[static_cast<int>(p)]; }
	
	private:
		std::string _q;
		CCategories _c;
		CPurities   _p;
	};
	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CCollectionRequestSettings
	{
	public:
		CCollectionRequestSettings();
		CCollectionRequestSettings(const CCollectionRequestSettings & src);
		CCollectionRequestSettings& operator=(const CCollectionRequestSettings & src);

	public:
		std::string userName() const { return _userName; }
		void setUserName( const std::string & u) { _userName = u; }

		std::string colId() const { return _colId; }
		void setColId(const std::string & i) { _colId = i; }
		
		CPurities purities() const { return _p; }
		void setPurities(const CPurities & p) { _p = p; }
		
		int  pageCount() const { return _pageCount; } // negative => unknown
		void setPageCount( unsigned int p) { _pageCount = static_cast<int>(p); }
		void resetPageCount() { _pageCount = -1; }
	
	private:
		std::string _userName;
		std::string _colId;
		CPurities   _p;
		int         _pageCount; // negative => unknown
	};
	
	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

	class CSession
	{
	public:
		CSession();
		~CSession();

	public:
		bool login( const std::string & username, const std::string & password);
		void logout();
		bool logged() const;
	
	public: // user infos
		std::string userUrl() const;
		std::string userName() const;
		std::map<std::string, std::string> userFavorites() const; // [id] = name
		
	public:
		std::string userUrl(const std::string & userName) const;
		std::map<std::string, std::string> userFavorites(const std::string & userName) const; // [id] = name
	
		bool updatePageCount(CCollectionRequestSettings & s) const;
		
		std::list<std::string> getRandomIds( const CRandomRequestSettings & s ) const;
		std::list<std::string> getRandomIds( const CCollectionRequestSettings & s ) const;
		std::list<std::string> getRandomIds( const std::string & user, const CPurities & p ) const;
		std::string getFullImageUrl(const std::string & imageId) const;
		
	private:
		class CImpl;
		std::auto_ptr<CImpl> _p;
		
	};
	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

	class CImageData
	{
	public:
		CImageData(const std::string & sId, const std::string & url)
		:	_sId(sId)
		,	_url(url)
		{}
		
	public:
		const std::string & sId() const { return _sId; }
		const std::string & url() const { return _url; }

	public:
		const std::vector<uint8_t> & getData() const { return _data; }
		std::vector<uint8_t> & getData() { return _data; }
		
	private:
		std::string          _sId;
		std::string          _url;
		std::vector<uint8_t> _data;
	};

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CImageProvider
	{
	public:
		CImageProvider();
		
	public:
		void start( const CRandomRequestSettings & rndAccess );
		void stop();
		CImageData* getNextImage();

	private:
		class CImpl;
		std::auto_ptr<CImpl> _p;
	};

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

}


//- common tools
#include "thread.h"

namespace wallhaven
{
	//- randoms
	double rnd();
	
	//- String tools
	
	std::string strGetLower(const std::string & s);
	std::string strGetLTrim(const std::string &s);
	std::string strGetRTrim(const std::string &s);
	std::string strGetTrim(const std::string &s);
	std::string strNoNewline(const std::string & s);

}
