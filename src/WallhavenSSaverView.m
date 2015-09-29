//
//  FraSaverView.m
//  FraSaver
//
//  Created by franck on 15/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//
#import "saverDelegate.h"


@interface WallhavenSSaverView ()

@end

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation WallhavenSSaverView {

	SaverDelegate * _saver;
}

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self)
	{
		[self setAnimationTimeInterval:1/48.0]; // en sec
		_saver = [[SaverDelegate alloc] initWithFrame:frame isPreview:isPreview saverView:self];
		_delegate = _saver;
	}

	return self;
}

- (void)startAnimation
{
    [super startAnimation];
	[_delegate startAnimation:self];
}

- (void)stopAnimation
{
    [super stopAnimation];
	[_delegate stopAnimation:self];
}
- (void)setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	[_delegate setFrameSize:self newSize:newSize];
}

- (void)drawRect:(NSRect)rect
{
	[super drawRect:rect];
	[_delegate drawRect:self rect:rect];
}

- (void)animateOneFrame
{
	[super animateOneFrame];
	[_delegate animateOneFrame:self];
}

- (BOOL)hasConfigureSheet
{
    return YES;
}

	// debug purpose only:
- (NSWindow*)configureSheetFromRig
{
	return [_delegate configureSheet:self fromRig:YES];
}

- (NSWindow*)configureSheet
{
	return [_delegate configureSheet:self fromRig:NO];
}


@end

