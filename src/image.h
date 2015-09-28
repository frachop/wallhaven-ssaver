//
//  image.h
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#pragma once
#import <cocoa/cocoa.h>
#import <OpenGL/gl.h>

@interface Image : NSObject 

@property (readonly, getter=getWidth ) NSUInteger width;
@property (readonly, getter=getHeight) NSUInteger height;

-(instancetype)initWithBytes:(const void*)data length:(unsigned long int)length;
-(GLuint) toTexture;

@end
