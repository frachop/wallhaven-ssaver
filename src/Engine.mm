//
//  Engine.m
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import "Engine.h"
#import "OglView.h"
#import <OpenGL/gl.h>
#import "imageQueue.h"
#import "curl.h"

template<class T> inline void _swap(T & a, T& b) { T tmp(a); a = b; b = tmp; }

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -

class CSheet
{
public:
	CSheet()
	:	_tId(0)
	,	_width(0)
	,	_height(0)
	{
	}

	CSheet(const CSheet & s)
	:	_tId(s._tId)
	,	_width(s._width)
	,	_height(s._height)
	{
	}

	GLuint texId() const { return _tId; }
	GLint width() const { return _width; }
	GLint height() const { return _height; }

	void clear()
	{
		if (_tId) {
			glDeleteTextures(1, &_tId);
		}
		_tId    = 0;
		_width = 0;
		_height= 0;
	}

	CSheet& operator=( const Image * i)
	{
		if (_tId) {
			glDeleteTextures(1, &_tId);
		}
		_tId    = [i toTexture];
		_width = (GLint)(i.width);
		_height= (GLint)(i.height);
	
		return (*this);
	}

	CSheet& operator=( const CSheet & s)
	{
		if (&s != this) {
			_tId   = s._tId;
			_width = s._width;
			_height= s._height;
		}
	
		return (*this);
	}
	
	void draw()
	{
		if (_tId == 0)
			return;
		
		glEnable( GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _tId);
		
		const float w = _width;
		const float h = _height;
		
		float u0 = 0, v0 = 0;
		float u1 = w, v1 = h;
		
		float x0 = 0, y0 = h;
		float x1 = w, y1 = 0;
		
		glBegin(GL_QUADS);
		{
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u0, v0); glVertex2f(x0, y0);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u0, v1); glVertex2f(x0, y1);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u1, v1); glVertex2f(x1, y1);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u1, v0); glVertex2f(x1, y0);
		}
		glEnd();
		
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		glDisable( GL_TEXTURE_RECTANGLE_ARB);
	}
	
	
private:
	GLuint _tId;
	GLint  _width;
	GLint  _height;

};

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -

class CAnimator
{
public:
	CAnimator()
	:	_zoomNominal(0)
	,	_started(false)
	{
		_imgSize = {0,0};
		_canevaSize = {0,0};
	}
	
	CAnimator(const CAnimator & s)
    :	_start      ( s._start)
	,	_canevaSize ( s._canevaSize)
	,	_imgSize    ( s._imgSize)
	,	_zoomNominal( s._zoomNominal)
	,	_center     ( s._center)
	,	_anchor     ( s._anchor)
	,	_started    ( s._started)
	,	_zoomMax    ( s._zoomMax)
	,	_msDuration ( s._msDuration)
	,	_msFadeDuration( s._msFadeDuration)
	,	_z0 ( s._z0)
	,	_z1 ( s._z1)
	,	_delta( s._delta)
	{
	}

	CAnimator & operator=(const CAnimator & s)
	{
		if (this != &s) {
    		_start      = s._start;
			_canevaSize = s._canevaSize;
			_imgSize    = s._imgSize;
			_zoomNominal= s._zoomNominal;
			_center     = s._center;
			_anchor     = s._anchor;
			_started    = s._started;
			_zoomMax    = s._zoomMax;
			_msDuration = s._msDuration;
			_msFadeDuration= s._msFadeDuration;
			
			_z0 = s._z0;
			_z1 = s._z1;
			_delta = s._delta;
		}
	
		return (*this);
	}
	
	void setCanevaSize( const NSSize & s)
	{
		_canevaSize = s;
		compute();
	}
	
	void start(const CSheet & img)
	{
		_imgSize = CGSizeMake(static_cast<CGFloat>(img.width()), static_cast<CGFloat>(img.height()));
	    _start = std::chrono::system_clock::now();
		
		compute();
		
		_zoomMax = (1.01 + (0.06*wallhaven::rnd())) * _zoomNominal;
		_msDuration    = 9000. + (2000. * wallhaven::rnd());
		_msFadeDuration= 2000. + (500. * wallhaven::rnd());
		_delta.x = 0; //rnd() * _canevaSize.width * 0.02 * ((rnd() > 0.5) ? 1.0 : -1.0);
		_delta.y = 0; //rnd() * _canevaSize.height * 0.02 * ((rnd() > 0.5) ? 1.0 : -1.0);
		
		_z0 = _zoomNominal;
		_z1 = _zoomMax    ;
		const bool bZoomOut = wallhaven::rnd() > 0.5;
		if (bZoomOut) std::swap( _z0, _z1 );
		
		_started = true;
		
	}
	
	bool started() const { return _started; }
	bool done() const
	{
		assert((_imgSize.width > 0.) && (_imgSize.height >0.) && _started);
		const auto end = std::chrono::system_clock::now();
		const float elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
		return elapsed_ms >= _msDuration;
	}

	void stop()
	{
		_started = false;
		_imgSize = CGSizeMake(0,0);
	}

	bool fading() const
	{
		const auto end = std::chrono::system_clock::now();
		float elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
		elapsed_ms = std::min(_msDuration, elapsed_ms);
		return (elapsed_ms >= _msDuration - _msFadeDuration);
	}

	float setupGL()
	{
		assert((_imgSize.width > 0.) && (_imgSize.height > 0.) && _started);
	
		const auto end = std::chrono::system_clock::now();
		float elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
		
		elapsed_ms = std::min(_msDuration, elapsed_ms);
		
		const float a = (elapsed_ms >= _msDuration - _msFadeDuration) ? (_msDuration - elapsed_ms) / _msFadeDuration : 1.f;
		
		if (a < 1.f)
		{
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1,1,1,a);
		}
		
		const float k = elapsed_ms / _msDuration;
		float z = _z0 + ((_z1 - _z0) * k);
		
		const NSPoint delta = CGPointMake( k * _delta.x, k * _delta.y);
		
		glPushMatrix();
		glTranslatef( _center.x + delta.x, _center.y - delta.y, 0 );
		glScalef    (z, z, 1);
		glTranslatef( -_anchor.x, -_anchor.y, 0 );
	
		return z;
	}

	void restoreGL()
	{
		glDisable (GL_BLEND);
		glColor4f(1,1,1,1);
		glPopMatrix();
	}

private:
	void compute()
	{
		_center = { 0.5f * _canevaSize.width, 0.5f * _canevaSize.height };
		_anchor = { 0.5f * _imgSize.width, 0.5f * _imgSize.height };
		if ((_imgSize.width == 0.) || (_imgSize.height == 0.))
			return;
		
		_zoomNominal = _canevaSize.width / _imgSize.width;
		if (_zoomNominal * _imgSize.height < _canevaSize.height)
			_zoomNominal = _canevaSize.height / _imgSize.height;
	}

private:
	std::chrono::time_point<std::chrono::system_clock> _start;

private:
	NSSize  _canevaSize;
	NSSize  _imgSize;
	float   _zoomNominal;
	NSPoint _center;
	NSPoint _anchor;

	bool  _started;
	float _zoomMax;
	float _msDuration;
	float _msFadeDuration;
	float _z0,_z1;
	NSPoint _delta;
};

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -

@implementation Engine
{
	FraSaverView * _view;

	OglView * oglView;
	
	ImageQueue * _imgQueue;
	CSheet _texCurrent, _texNext;
	CAnimator _animCurrent, _animNext;
	NSSize _frameSize;
	
	CCurlLibrary _curl;

}

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview fraSaverView:(FraSaverView*)view
{
    self = [super init];

    if (self) 
	{
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
		[_view setAnimationTimeInterval: [self getAnimationTimeInterval]]; // en sec
	}

	_imgQueue = [[ImageQueue alloc] init];
	return self;
}


-(NSTimeInterval) getAnimationTimeInterval
{
	return 3 / 1.;
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
}

- (void)stopAnimation:(ScreenSaverView*)sender
{
	assert( sender == _view );
}

- (void)drawRect:(ScreenSaverView*)sender rect:(NSRect)rect
{
	assert( sender == _view );
	[[oglView openGLContext] makeCurrentContext];
	
	glClear(GL_COLOR_BUFFER_BIT);
	if (_texCurrent.texId() == 0)
		return;
	
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



@end
