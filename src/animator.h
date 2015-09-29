//
//  animator.hpp
//  wallhaven-ssaver
//
//  Created by franck on 29/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#pragma once
#include "common.h"
#import <Foundation/Foundation.h>
#import "textureSheet.h"


class CAnimator
{
public:
	CAnimator();
	CAnimator(const CAnimator & s);
	CAnimator & operator=(const CAnimator & s);

	void setCanevaSize( const NSSize & s);
	void start(const CTextureSheet & img);
	bool started() const { return _started; }
	bool done() const;
	void stop();

	bool fading() const;

	float setupGL();
	void restoreGL();

private:
	void compute();

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
