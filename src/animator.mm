//
//  animator.cpp
//  wallhaven-ssaver
//
//  Created by franck on 29/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#include "animator.h"

CAnimator::CAnimator()
:	_zoomNominal(0)
,	_started(false)
{
	_imgSize = {0,0};
	_canevaSize = {0,0};
}

CAnimator::CAnimator(const CAnimator & s)
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

CAnimator & CAnimator::operator=(const CAnimator & s)
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

void CAnimator::setCanevaSize( const NSSize & s)
{
	_canevaSize = s;
	compute();
}

void CAnimator::start(const CTextureSheet & img)
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

bool CAnimator::done() const
{
	assert((_imgSize.width > 0.) && (_imgSize.height >0.) && _started);
	const auto end = std::chrono::system_clock::now();
	const float elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
	return elapsed_ms >= _msDuration;
}

void CAnimator::stop()
{
	_started = false;
	_imgSize = CGSizeMake(0,0);
}

bool CAnimator::fading() const
{
	const auto end = std::chrono::system_clock::now();
	float elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start).count();
	elapsed_ms = std::min(_msDuration, elapsed_ms);
	return (elapsed_ms >= _msDuration - _msFadeDuration);
}

float CAnimator::setupGL()
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

void CAnimator::restoreGL()
{
	glDisable (GL_BLEND);
	glColor4f(1,1,1,1);
	glPopMatrix();
}

void CAnimator::compute()
{
	_center = { 0.5f * _canevaSize.width, 0.5f * _canevaSize.height };
	_anchor = { 0.5f * _imgSize.width, 0.5f * _imgSize.height };
	if ((_imgSize.width == 0.) || (_imgSize.height == 0.))
		return;
	
	_zoomNominal = _canevaSize.width / _imgSize.width;
	if (_zoomNominal * _imgSize.height < _canevaSize.height)
		_zoomNominal = _canevaSize.height / _imgSize.height;
}

