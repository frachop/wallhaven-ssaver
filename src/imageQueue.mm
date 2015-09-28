//
//  imageQueue.cpp
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import "imageQueue.h"
#import "common.h"
#import "parser.h"

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

class CImageProvider
:	private wallhaven::CThread
{
public:
	CImageProvider();
	~CImageProvider();

public:
	void start();
	void abort();
	Image* getNextImage();

protected:
	virtual void run() override;
	
private:
	std::mutex        _mutex;
	std::list<Image*> _images;
	
	CIds _ids;
	
};

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

CImageProvider::CImageProvider()
{
}

CImageProvider::	~CImageProvider()
{
}

void CImageProvider::start()
{
	CThread::start();
	_ids.start();
}

void CImageProvider::abort()
{
	_ids.stop();
	CThread::abort();
}

void CImageProvider::run()
{
	float prc;
	
	while (canContinue())
	{
		_mutex.lock();
		const size_t cnt = _images.size();
		_mutex.unlock();
		if (cnt < 4)
		{
			const std::string url = _ids.getNextUrl();
			if (!url.empty()) {

				NSLog(@"%s - downloading (%s) (%zu images)", __PRETTY_FUNCTION__, url.c_str(), cnt);
				wallhaven::CImageDownloader dl;
				dl.start(url);
				while (canContinue()) {
					if (!dl.getProgress(prc)) {
						const std::vector<uint8_t> & imgData = dl.getBuffer();
						if (imgData.size()) {
							Image * img = [[Image alloc] initWithBytes:imgData.data() length:imgData.size()];
							_mutex.lock();
							_images.push_back(img);
							NSLog(@"%s - %s downloaded (%zu images)", __PRETTY_FUNCTION__, url.c_str(), _images.size());
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

	NSLog(@"%s - DONE", __PRETTY_FUNCTION__);
}

Image* CImageProvider::getNextImage()
{
	Image * res = nil;
	_mutex.lock();
	if (!_images.empty()) {
		res = _images.front();
		_images.pop_front();
		//NSLog(@"%s : %@", __PRETTY_FUNCTION__, res.sID);
	}
	_mutex.unlock();
	
	return res;
}

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation ImageQueue
{
	CImageProvider _imgs;
}

-(instancetype)init
{
	self = [super init];
	if (self)
	{
		_imgs.start();
	}
	return self;
}

-(void)dealloc
{
	NSLog(@"%s", __PRETTY_FUNCTION__);
}

-(void)start
{
}

-(Image*)getNextImage
{
	return _imgs.getNextImage();
}

-(void)stop
{
}

@end

