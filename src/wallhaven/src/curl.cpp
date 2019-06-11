#include "curl.h"
#include <iostream>
#include "internals.h"

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

CCurlLibrary::CCurlLibrary()
{
	LOGT("{}", __PRETTY_FUNCTION__);
	CURLcode c = curl_global_init(CURL_GLOBAL_ALL);
	if (c) {
		std::cerr << "can't init curl" << std::endl;
		exit(EXIT_FAILURE);
	}
}

CCurlLibrary::~CCurlLibrary()
{
	curl_global_cleanup();
}

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

CCurl::CCurl()
:	_p(nullptr)
{
	
	static CCurlLibrary _curl;

	_p= curl_easy_init();
}

CCurl::~CCurl()
{
	if (_p) curl_easy_cleanup(_p);
}

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string CCurl::escapeString( const std::string & src) const
{
	assert(_p);
	std::string result;
	char * p = curl_easy_escape( _p , src.c_str() , static_cast<int>(src.length()) );
	if (p) {
		result = p;
		curl_free(p);
	}
	return result;
}

size_t CCurl::wfString(void *ptr, size_t size, size_t nmemb, std::string * s)
{
	std::vector<char> v;
	v.resize(1 + size*nmemb);
	memcpy( v.data(), ptr, size*nmemb);
	v.push_back('\0');
	(*s) += v.data();
	return size*nmemb;
}

CURLcode CCurl::perform()
{
	assert(_p);
	
	const CURLcode resCode = curl_easy_perform(_p);
	if (resCode != CURLE_OK)
		std::cerr << "curl error code " << resCode << " " << curl_easy_strerror(resCode) << std::endl;
	
	return resCode;
}

CURLcode CCurl::perform(std::string & response)
{
	assert(_p);
	response.clear();
	setopt(CURLOPT_WRITEFUNCTION, wfString);
 	setopt(CURLOPT_WRITEDATA, &response);
	const CURLcode res= perform();
	
	_httpResponseCode= 0;
	curl_easy_getinfo(_p, CURLINFO_RESPONSE_CODE, &_httpResponseCode);
	
	return res;
}




