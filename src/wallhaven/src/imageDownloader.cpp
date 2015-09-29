//
//  imageDownloader.cpp
//  FraSaver
//
//  Created by franck on 28/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#include "imageDownloader.h"
#include <iostream>
#include <sstream>

namespace wallhaven {

	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

	CImageDownloader::CImageDownloader()
	:	_curl(nullptr)
	{
	}
	
	CImageDownloader::~CImageDownloader()
	{
	}
	
	size_t CImageDownloader::wCallbackEntryPoint(char *ptr, size_t size, size_t nmemb, CImageDownloader * me)
	{
		assert( me );
		if (size * nmemb)
			return me->wCallback(ptr, size, nmemb);
		return 0;
	}
	
	static void _parseResponseHeader( std::map<std::string, std::string> & headerMap, const std::string & result)
	{
		headerMap.clear();
		
		std::string line;
		std::istringstream isstream(result);
		while (std::getline(isstream, line))
		{
			const std::string::size_type i = line.find( ':' );
			if (i == std::string::npos)
				continue;
			
			const std::string left = strGetLower(strGetTrim(line.substr(0,i)));
			const std::string right= strGetTrim(line.substr(i+1));
			if (left.empty())
				continue;
			
			if (headerMap.find(left) != headerMap.end()) {
				//std::cout << "WARNING : Duplicate header keyword " << left << std::endl;
			} else {
				headerMap[left] = right;
			}
		}
	}
	
	
	size_t CImageDownloader::wCallback(char *ptr, size_t size, size_t nmemb)
	{
		//std::cout << __PRETTY_FUNCTION__ << std::endl;
		if (!_headerDone) {
			//std::cout << __PRETTY_FUNCTION__ << ": headerDone" << std::endl;
			_headerDone = true;
			std::map<std::string, std::string> headerMap;
			_parseResponseHeader( headerMap, _headerResponse);
			
			const auto i = headerMap.find("content-length");
			if (i != headerMap.end()) {
				
				std::istringstream iss(i->second);
				uintmax_t value;
				iss >> value;
				if (!iss.fail() )
					_fileLength= value;
				else {
					std::cerr << "can't get file length [url:" << _url << "]" << std::endl;
					_fileLength = 0;
				}
			}
			
			if (_fileLength && (_fileLength != _buffer.size()))
				_buffer.resize(_fileLength);
		}
		if ((_fileLength == 0) || (isAborted())) {
			
			std::cerr << __PRETTY_FUNCTION__ << ": file length == 0 or Aborted " << std::endl;
			return 0;
		}
		
		memcpy( _buffer.data() + _dlLen, ptr, size*nmemb );
		_dlLen += size * nmemb;
		
		//std::cout << fmt::format("write_callback  - {}x{} = {} ({})", size, nmemb, size * nmemb, _len) << std::endl;
		return size * nmemb;
	}
	
	
	int CImageDownloader::wProgressCallbackEntryPoint(CImageDownloader *me,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow)
	{
		assert( me );
		return me->wProgressCallback(dltotal, dlnow);
	}
	
	int CImageDownloader::wProgressCallback(curl_off_t dltotal, curl_off_t dlnow)
	{
		//std::cout << __PRETTY_FUNCTION__ << ": " << dlnow << " / " << dltotal << std::endl;
		return 0;
	}
	
	void CImageDownloader::run()
	{
		_headerResponse.clear();
		
		_curl = new CCurl( );
		_curl->setopt(CURLOPT_URL, _url.c_str());
		//setopt(CURLOPT_USERAGENT, "frasaver-agent/1.0");
		
		_curl->setopt(CURLOPT_HEADER, 0L);
		_curl->setopt(CURLOPT_HEADERFUNCTION, CCurl::wfString);
		_curl->setopt(CURLOPT_HEADERDATA, &_headerResponse);
		
		_curl->setopt(CURLOPT_WRITEFUNCTION, wCallbackEntryPoint);
		_curl->setopt(CURLOPT_WRITEDATA, static_cast<void*>(this));
		
		_curl->setopt(CURLOPT_NOPROGRESS, 0);
		_curl->setopt(CURLOPT_XFERINFODATA, static_cast<void*>(this));
		_curl->setopt(CURLOPT_XFERINFOFUNCTION, wProgressCallbackEntryPoint);
		
		const CURLcode res = _curl->perform();
		if (res != 0)
			_dlLen = 0;
		
		delete _curl; _curl = nullptr;
	}
	
	bool CImageDownloader::start(const std::string & url)
	{
		assert( _curl == nullptr );
		
		_url = url;
		_fileLength = 0;
		_dlLen = 0;
		_headerDone = false;
		
		CThread::start();
		return true;
	}
	
	bool CImageDownloader::getProgress( float & prc )
	{
		prc = 0.f ;
		if ( !_headerDone )
			return true;
		
		const uintmax_t lg  = _fileLength;
		const uintmax_t done= _dlLen;
		
		if (lg == 0)
			return false;
		
		prc= (100.f * static_cast<float>(done) / static_cast<float>(lg));
		const bool res= done < lg;
		
		if (!res)
			abort();
		
		return res;
	}

}
