//
//  FraSaverView.m
//  FraSaver
//
//  Created by franck on 15/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//
#import "Engine.h"


@interface FraSaverView () <ConfigurationControllerDelegate>

@end

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation FraSaverView {

	Engine * _engine;
}

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self)
	{
		//NSLog(@"%@", [[NSBundle mainBundle] bundlePath]);
	
		NSBundle * bundle = [NSBundle bundleForClass:[self class]];
		NSUserDefaults *prefs = [ScreenSaverDefaults defaultsForModuleWithName:[bundle bundleIdentifier]];
		_configController = [[ConfigurationController alloc] initWithUserDefaults:prefs];
		_configController.delegate = self;
	
		_engine = [[Engine alloc] initWithFrame:frame isPreview:isPreview fraSaverView:self];
		_delegate = _engine;
		[self setAnimationTimeInterval:1/60.0]; // en sec
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

- (NSWindow*)configureSheet
{
	return [_configController configureSheet];
}

- (void)configController:(ConfigurationController *)configController dismissConfigSheet:(NSWindow *)sheet {
  //if (_isPreview) { [self loadFromStart]; }
	
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	[[NSApplication sharedApplication] endSheet:sheet];
#pragma GCC diagnostic pop
	}


@end

