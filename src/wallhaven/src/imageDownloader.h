//
//  imageDownloader.h
//  wallhaven-ssaver
//
//  Created by franck on 29/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#pragma once
#include "../include/wallhaven.h"
#include "curl.h"

namespace wallhaven {

	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CImageDownloader
	:	private CThread
	{
	public:
		CImageDownloader();
		~CImageDownloader();
		bool start(const std::string & url);
		bool getProgress( float & prc );
		void abort() { CThread::abort(); }
		const std::vector<uint8_t>& getBuffer() const { return _buffer; }
		
	private:
		static size_t wCallbackEntryPoint(char *ptr, size_t size, size_t nmemb, CImageDownloader *me);
		size_t wCallback(char *ptr, size_t size, size_t nmemb);
		
		static int wProgressCallbackEntryPoint(CImageDownloader *me,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow);
		int wProgressCallback(curl_off_t dltotal, curl_off_t dlnow);
		
		virtual void run() override;
		
	private:
		CCurl * _curl;
		
	private:
		std::string          _url;
		std::vector<uint8_t> _buffer;
		std::string          _headerResponse;
		
	private:
		std::atomic_bool       _headerDone;
		std::atomic<uintmax_t> _fileLength;
		std::atomic<uintmax_t> _dlLen;
	};

}