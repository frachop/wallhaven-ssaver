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
	
	void CUrlProvider::start( const CRandomRequestSettings & rrs )
	{
		abort();
		_rrs = rrs;
		CThread::start();
	}
	
	CImageInfo * CUrlProvider::getNextUrl()
	{
		CImageInfo * result(nullptr);
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
		
		while (canContinue())
		{
			if (ids.size() < 24) {
				
				//NSLog(@"%s - (%zu) load new Ids ... ", __PRETTY_FUNCTION__, ids.size());
				
				const std::list<std::string> randoms = session.getRandomIds( _rrs );
				for (const auto i: randoms)
					ids.push_back(i);

				//NSLog(@"%s - got (%zu) Ids ... ", __PRETTY_FUNCTION__, ids.size());
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
					_urls.push_back(new CImageInfo( i, fullUrl));
					shuffle( _urls );
					//NSLog(@"%s - full url (%s) found (%zu urls)", __PRETTY_FUNCTION__, i.c_str(), _fullUrls.size());
					_mutex.unlock();
				}
				
			}
			ms_sleep(500);
		}
	}
	
	

}