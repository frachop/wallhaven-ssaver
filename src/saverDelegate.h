//
//  Engine.h
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WallhavenSSaverView.h"

@interface SaverDelegate : NSObject<WallhavenSSaverViewDelegate>

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview saverView:(WallhavenSSaverView*)view;

@end
