//
//  image.cpp
//  FraSaver
//
//  Created by franck on 16/09/2015.
//  Copyright (c) 2015 Frachop. All rights reserved.
//

#import "image.h"
#include "common.h"

@implementation Image
{
	NSData* _data;
	NSBitmapImageRep * _rep;

}

-(instancetype)initWithBytes:(const void*)data length:(unsigned long int)length
{
	self = [super init];
	if (self)
	{
		_data= [[NSData alloc] initWithBytes:data length:length];
		_rep = [[NSBitmapImageRep alloc] initWithData:_data];
	}
	return self;
}

-(void)dealloc
{
}

-(NSUInteger)getWidth
{
	return [_rep pixelsWide];
}

-(NSUInteger)getHeight
{
	return [_rep pixelsHigh];
}

-(GLuint) toTexture
{
	const NSUInteger w = self.width;
	const NSUInteger h = self.height;
	const GLenum target = GL_TEXTURE_RECTANGLE_ARB;
	
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glBindTexture(target, tex_id);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glTexImage2D(target, 0, GL_RGBA, (GLsizei)w, (GLsizei)h, 0, GL_RGBA, GL_UNSIGNED_BYTE, [_rep bitmapData]);
	glGenerateMipmap(target);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glBindTexture(target, 0);
	return tex_id;
}


@end;