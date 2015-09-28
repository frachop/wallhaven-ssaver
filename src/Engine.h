//
//  Engine.h
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FraSaverView.h"

@interface Engine : NSObject<FraSaverViewDelegate>

@property (readonly, getter=getAnimationTimeInterval) NSTimeInterval animationTimeInterval;

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview fraSaverView:(FraSaverView*)view;

@end
