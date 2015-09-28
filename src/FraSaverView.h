//
//  FraSaverView.h
//  FraSaver
//
//  Created by franck on 15/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import <ScreenSaver/ScreenSaver.h>
#import "ConfigurationController.h"

@protocol FraSaverViewDelegate;

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface FraSaverView : ScreenSaverView

@property (nonatomic, assign) id <FraSaverViewDelegate> delegate;
@property (nonatomic, strong) ConfigurationController * configController;

@end

//- /////////////////////////////////////////////////////////////////////////////////////////////////////////

@protocol FraSaverViewDelegate

- (void)setFrameSize:(ScreenSaverView*)sender newSize:(NSSize)newSize;
- (void)startAnimation:(ScreenSaverView*)sender;
- (void)stopAnimation:(ScreenSaverView*)sender;
- (void)drawRect:(ScreenSaverView*)sender rect:(NSRect)rect;
- (void)animateOneFrame:(ScreenSaverView*)sender;

@end
