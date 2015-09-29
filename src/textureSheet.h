//
//  textureSheet.hpp
//  wallhaven-ssaver
//
//  Created by franck on 29/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#pragma once
#import <OpenGL/gl.h>
#import "imageQueue.h"

class CTextureSheet
{
public:
	CTextureSheet();
	CTextureSheet(const CTextureSheet & s);

public:
	GLuint texId() const { return _tId; }
	GLint width() const { return _width; }
	GLint height() const { return _height; }

public:
	void clear();
	CTextureSheet& operator=( const Image * i);
	CTextureSheet& operator=( const CTextureSheet & s);
	void draw();
	
private:
	GLuint _tId;
	GLint  _width;
	GLint  _height;

};

