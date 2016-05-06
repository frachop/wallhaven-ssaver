//
//  imageQueue.cpp
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import "imageQueue.h"
#import "common.h"

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation ImageQueue
{
	wallhaven::CImageProvider _imgProvider;
}

-(instancetype)init
{
	self = [super init];
	if (self)
	{
	}
	return self;
}

-(void)dealloc
{
	NSLog(@"%s", __PRETTY_FUNCTION__);
}

-(void)startWithRndSettings:(const wallhaven::CRandomRequestSettings & )settings
{
	_imgProvider.start(settings);
}

-(Image*)getNextImage
{
	wallhaven::CImageData * i = _imgProvider.getNextImage();
	if (i == nullptr)
		return nullptr;

	Image * img = [[Image alloc] initWithBytes:i->getData().data() length:i->getData().size()];
	delete i;

	return img;
}

-(void)stop
{
	_imgProvider.stop();
}

@end

