//
//  FraSaverView.h
//  FraSaver
//
//  Created by franck on 15/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import <ScreenSaver/ScreenSaver.h>

@protocol WallhavenSSaverViewDelegate;

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface WallhavenSSaverView : ScreenSaverView

@property (nonatomic, assign) id <WallhavenSSaverViewDelegate> delegate;

// debug purpose only:
- (NSWindow*)configureSheetFromRig;

@end

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@protocol WallhavenSSaverViewDelegate

- (void)setFrameSize:(ScreenSaverView*)sender newSize:(NSSize)newSize;
- (void)startAnimation:(ScreenSaverView*)sender;
- (void)stopAnimation:(ScreenSaverView*)sender;
- (void)drawRect:(ScreenSaverView*)sender rect:(NSRect)rect;
- (void)animateOneFrame:(ScreenSaverView*)sender;

- (NSWindow*)configureSheet:(ScreenSaverView*)sender fromRig:(BOOL)fromRig;

@end
