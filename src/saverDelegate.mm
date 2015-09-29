//
//  Engine.m
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import "saverDelegate.h"
#import "OglView.h"
#import "textureSheet.h"
#import "imageQueue.h"
#import "animator.h"
#import "ConfigurationController.h"

template<class T> inline void _swap(T & a, T& b) { T tmp(a); a = b; b = tmp; }

@interface SaverDelegate () <ConfigurationControllerDelegate>

@end


//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -

@implementation SaverDelegate
{
	ConfigurationController * _configController;

	// debug purpose only:
	BOOL _fromRig;

	WallhavenSSaverView * _view;
	Configuration * _config;

	OglView * oglView;
	
	ImageQueue * _imgQueue;
	CTextureSheet _texCurrent, _texNext;
	CAnimator _animCurrent, _animNext;
	NSSize _frameSize;
}

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview saverView:(WallhavenSSaverView*)view
{
    self = [super init];

    if (self) 
	{
		_fromRig = NO;
		_view = view;
		NSOpenGLPixelFormatAttribute attributes[] = { 0 };
		NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
		oglView = [[OglView alloc] initWithFrame:NSZeroRect pixelFormat:format];

		if (!oglView)
		{             
			NSLog( @"Couldn't initialize OpenGL view." );
			return nil;
		} 

		_frameSize = frame.size;
		_animCurrent.setCanevaSize(_frameSize);
		_animNext.setCanevaSize(_frameSize);
		[_view addSubview:oglView];
		[self setUpOpenGL];
		
		NSBundle * bundle = [NSBundle bundleForClass:[self class]];
		NSUserDefaults *prefs = [ScreenSaverDefaults defaultsForModuleWithName:[bundle bundleIdentifier]];
		_config = [[Configuration alloc] initWithUserDefaults:prefs];
	
	}

	_imgQueue = [[ImageQueue alloc] init];
	return self;
}

- (void)setUpOpenGL
{  
	[[oglView openGLContext] makeCurrentContext];
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
}


- (void)setFrameSize:(ScreenSaverView*)sender newSize:(NSSize)newSize
{
	assert( sender == _view );
	
	_frameSize = newSize;

	[oglView setFrameSize:newSize];
	[[oglView openGLContext] makeCurrentContext];
	
	// Reshape
	glViewport( 0, 0, (GLsizei)newSize.width, (GLsizei)newSize.height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0,(GLsizei)newSize.width, 0, (GLsizei)newSize.height, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	_animCurrent.setCanevaSize(_frameSize);
	_animNext.setCanevaSize(_frameSize);
	
	[[oglView openGLContext] update];
}

- (void)startAnimation:(ScreenSaverView*)sender
{
	assert( sender == _view );
	[_imgQueue startWithRndSettings:_config.randomRequest];
}

- (void)stopAnimation:(ScreenSaverView*)sender
{
	assert( sender == _view );
	[_imgQueue stop];
}

- (void)restart
{
	[self stopAnimation:_view];
	[[oglView openGLContext] makeCurrentContext];
	_texCurrent.clear();
	_texNext.clear();
	_animCurrent.stop();
	_animNext.stop();
	[self startAnimation:_view];
	[_view setNeedsDisplay:YES];
}

- (void)drawRect:(ScreenSaverView*)sender rect:(NSRect)rect
{
	assert( sender == _view );
	[[oglView openGLContext] makeCurrentContext];
	
	glClear(GL_COLOR_BUFFER_BIT);
	if (_texCurrent.texId() == 0) {
		glFlush();
		return;
	}
	
	if (_animNext.started())
	{
		_animNext.setupGL();
		_texNext.draw();
		_animNext.restoreGL();
	}
	if (_animCurrent.started())
	{
		_animCurrent.setupGL();
		_texCurrent.draw();
		_animCurrent.restoreGL();
	}
	glFlush();
}

- (void)animateOneFrame:(ScreenSaverView*)sender
{
	assert( sender == _view );
	[[oglView openGLContext] makeCurrentContext];
	
	if ( _texCurrent.texId() == 0) {
		Image * img = [_imgQueue getNextImage];
		if (img == nil)
			return;

		NSLog(@"%s assign current ", __PRETTY_FUNCTION__);
		_texCurrent = img;
	}

	if ( _texNext.texId() == 0) {
		Image * img = [_imgQueue getNextImage];
		if (img) {
			NSLog(@"%s assign next ", __PRETTY_FUNCTION__);
			_texNext = img;
		}
	}

	assert( _texCurrent.texId() > 0 );
	
	if (!_animCurrent.started())
	{
		_animCurrent.start(_texCurrent);
	}
	else
	{
		if (_animCurrent.fading()) {
		
			if (!_animNext.started())
			{
				if (_texNext.texId() > 0)
				{
					_animNext.start(_texNext);
				}
			}
		}
	
		if ( _animCurrent.done())
		{
			_animCurrent.stop();
			_texCurrent.clear();
			_swap(_texNext, _texCurrent);
			_swap(_animNext, _animCurrent);
		}
	}
	
	// Redraw
	[_view setNeedsDisplay:YES];
}

- (NSWindow*)configureSheet:(ScreenSaverView*)sender fromRig:(BOOL)fromRig
{
	_fromRig = fromRig;
	if (_configController == nullptr) {
		_configController = [[ConfigurationController alloc] init:_config];
		_configController.delegate = self;
	}
	
	return [_configController configureSheet];
}

- (void)configController:(ConfigurationController *)configController dismissConfigSheet:(NSWindow *)sheet {

	if (_fromRig)
	{
		[sheet close];
	
	} else {
	
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		[[NSApplication sharedApplication] endSheet:sheet];
#pragma GCC diagnostic pop

	}

	[self restart];
}

@end
