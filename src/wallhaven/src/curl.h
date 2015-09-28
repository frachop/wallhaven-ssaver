#pragma once

#include "../include/wallhaven.h"
#include <curl/curl.h>
#include <cassert>

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCurlLibrary
{
public:
	CCurlLibrary();
	virtual ~CCurlLibrary();
};

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCurl
{
public:
	CCurl();
	virtual ~CCurl();
	operator CURL *() const { return _p; }
	void reset( ) const { assert( _p); curl_easy_reset(_p); }

public:
	template<typename T> CURLcode setopt(CURLoption option, T v) const;
	CURLcode perform(); 
	CURLcode perform(std::string & response);
	long  getHttpResponseCode() const { return _httpResponseCode; }

public:
	std::string escapeString( const std::string & src) const;

public:
	static size_t wfString(void *ptr, size_t size, size_t nmemb, std::string * s);
	
private:
	CURL * _p;
	long   _httpResponseCode;
};

template<>
inline CURLcode CCurl::setopt<std::string>(CURLoption option, std::string v) const
{
	assert(_p);
	return curl_easy_setopt(_p, option, v.c_str());
}


template<typename T>
inline CURLcode CCurl::setopt(CURLoption option, T v) const
{
	assert(_p);
	return curl_easy_setopt(_p, option, v);
}

