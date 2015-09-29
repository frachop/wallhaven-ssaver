//
//  textureSheet.cpp
//  wallhaven-ssaver
//
//  Created by franck on 29/09/2015.
//  Copyright © 2015 Frachop. All rights reserved.
//

#import "textureSheet.h"

CTextureSheet::CTextureSheet()
:	_tId(0)
,	_width(0)
,	_height(0)
{
}

CTextureSheet::CTextureSheet(const CTextureSheet & s)
:	_tId(s._tId)
,	_width(s._width)
,	_height(s._height)
{
}

void CTextureSheet::clear()
{
	if (_tId) {
		glDeleteTextures(1, &_tId);
	}
	_tId    = 0;
	_width = 0;
	_height= 0;
}

CTextureSheet& CTextureSheet::operator=( const Image * i)
{
	if (_tId) {
		glDeleteTextures(1, &_tId);
	}
	_tId    = [i toTexture];
	_width = (GLint)(i.width);
	_height= (GLint)(i.height);
	
	return (*this);
}

CTextureSheet& CTextureSheet::operator=( const CTextureSheet & s)
{
	if (&s != this) {
		// not deleting texture because of swap
		_tId   = s._tId;
		_width = s._width;
		_height= s._height;
	}
	
	return (*this);
}

void CTextureSheet::draw()
{
	if (_tId == 0)
		return;
	
	glEnable( GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _tId);
	
	const float w = _width;
	const float h = _height;
	
	float u0 = 0, v0 = 0;
	float u1 = w, v1 = h;
	
	float x0 = 0, y0 = h;
	float x1 = w, y1 = 0;
	
	glBegin(GL_QUADS);
	{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u0, v0); glVertex2f(x0, y0);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u0, v1); glVertex2f(x0, y1);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u1, v1); glVertex2f(x1, y1);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,u1, v0); glVertex2f(x1, y0);
	}
	glEnd();
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glDisable( GL_TEXTURE_RECTANGLE_ARB);
}

