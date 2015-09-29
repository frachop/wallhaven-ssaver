//
//  imageProvider.cpp
//  FraSaver
//
//  Created by franck on 28/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#include "urlProvider.h"
#include "imageDownloader.h"
#include "internals.h"


namespace wallhaven {
	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	class CImageProvider::CImpl
	:	public CThread
	{
	public:
		CImpl();
		~CImpl();
		
	public:
		void start( const CRandomRequestSettings & rndAccess );
		void stop();
		CImageData* getNextImage();

	protected:
		virtual void run() override;
		
	private:
		CRandomRequestSettings _rndAccess;
		std::mutex             _mutex;
		std::list<CImageData*> _images;
	};

	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////


	CImageProvider::CImpl::CImpl()
	{
	}

	CImageProvider::CImpl::~CImpl()
	{
		for (auto i: _images)
			delete i;
	}

	void CImageProvider::CImpl::start( const CRandomRequestSettings & rndAccess )
	{
		stop();
		_rndAccess = rndAccess;
		CThread::start();
	}
	
	void CImageProvider::CImpl::stop()
	{
		CThread::abort();
		for (auto i: _images)
			delete i;
		
		_images.clear();
	}
	
	CImageData* CImageProvider::CImpl::getNextImage()
	{
		CImageData* result(nullptr);
		_mutex.lock();
		if (!_images.empty()) {
			result = _images.front();
			_images.pop_front();
		}
		_mutex.unlock();
		return result;
	}

	void CImageProvider::CImpl::run()
	{
		LOGT("{} starts", __PRETTY_FUNCTION__);
	
		float prc;
		CUrlProvider urlProvider;
		urlProvider.start(_rndAccess);
		
		while (canContinue())
		{
			_mutex.lock();
			const size_t cnt = _images.size();
			_mutex.unlock();
			if (cnt < 4)
			{
				const CImageInfo info = urlProvider.getNextUrl();
				if (info.isValid()) {
					
					wallhaven::CImageDownloader dl;
					dl.start(info.url());
					while (canContinue()) {
						if (!dl.getProgress(prc)) {
							const std::vector<uint8_t> & imgData = dl.getBuffer();
							if (imgData.size()) {
								CImageData * img = new CImageData(info.sId(), info.url());
								img->getData() = imgData;
								_mutex.lock();
								_images.push_back(img);
								_mutex.unlock();
							}
							break;
						}
						ms_sleep(100);
					}
				}
			}
			
			ms_sleep(500);
		}
		urlProvider.stop();
		LOGT("{} stops", __PRETTY_FUNCTION__);
	}
	
	//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	CImageProvider::	CImageProvider()
	:	_p(new CImpl)
	{}
		
	void CImageProvider::start( const CRandomRequestSettings & rndAccess ) {
		_p->start(rndAccess); }

	void CImageProvider::stop() {
		_p->stop(); }
	
	CImageData* CImageProvider::getNextImage() {
		return _p->getNextImage(); }


}

