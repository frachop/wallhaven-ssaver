//
//  urlProvider.cpp
//  FraSaver
//
//  Created by franck on 28/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#include "urlProvider.h"
#include "internals.h"


namespace wallhaven {

	CImageInfo::CImageInfo()
	{
	}
	
	CImageInfo::CImageInfo(const std::string & sId, const std::string url)
	:	_sId(sId)
	,	_url(url)
	{
	}
	
	CImageInfo::CImageInfo(const CImageInfo& src)
	:	_sId(src._sId)
	,	_url(src._url)
	{
	}
	
	CImageInfo & CImageInfo::operator=(const CImageInfo& src)
	{
		if (this != &src)
		{
			_sId= src._sId;
			_url= src._url;
		}
		return (*this);
	}


	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////


	CUrlProvider::CUrlProvider()
	:	CThread()
	{
	}
	
	CUrlProvider::~CUrlProvider()
	{
		abort();
	}
	
	void CUrlProvider::start( const CRandomRequestSettings & rrs )
	{
		abort();
		_rrs = rrs;
		CThread::start();
	}

	void CUrlProvider::stop( )
	{
		CThread::abort();
		_urls.clear();
	}
	
	CImageInfo CUrlProvider::getNextUrl()
	{
		CImageInfo result;
		_mutex.lock();
		if (!_urls.empty()) {
			result = _urls.front();
			_urls.pop_front();
		}
		_mutex.unlock();
		return result;
	}

	
	void CUrlProvider::run()
	{
		CSession session;
		std::list<std::string> ids;
		
		//_rrs.set("movies");
		
		LOGT("{} starts", __PRETTY_FUNCTION__);
		
		while (canContinue())
		{
			if (ids.size() < 24) {
				
				const std::list<std::string> randoms = session.getRandomIds( _rrs );
				for (const auto i: randoms)
					ids.push_back(i);
			}
			
			_mutex.lock();
			const size_t fullUrlCount = _urls.size();
			_mutex.unlock();
			
			if ((!ids.empty()) && (fullUrlCount < 3)) {
				
				std::string i = ids.front();
				ids.pop_front();
				
				
				//NSLog(@"%s - looking for full url of (%s)", __PRETTY_FUNCTION__, i.c_str());
				const std::string fullUrl = session.getFullImageUrl(i);
				if (!fullUrl.empty()) {
					_mutex.lock();
					_urls.push_back(CImageInfo( i, fullUrl));
					shuffle( _urls );
					//NSLog(@"%s - full url (%s) found (%zu urls)", __PRETTY_FUNCTION__, i.c_str(), _fullUrls.size());
					_mutex.unlock();
				}
				
			}
			ms_sleep(500);
		}
		LOGT("{} stops", __PRETTY_FUNCTION__);
	}
	
	

}