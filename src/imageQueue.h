//
//  imageQueue.h
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#pragma once
#import "image.h"
#import "Configuration.h"


@interface ImageQueue : NSObject

-(instancetype)init;

-(void)start;
-(Image*)getNextImage;
-(void)stop;

@end



