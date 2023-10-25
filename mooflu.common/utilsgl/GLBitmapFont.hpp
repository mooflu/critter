#pragma once
// Description:
//   Just like a Bitmap Collection but tailored for a bitmap font.
//
// Copyright (C) 2001 Frank Becker
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation;  either version 2 of the License,  or (at your option) any  later
// version.
//
// This program is distributed in the hope that it will be useful,  but  WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details
//

#include "GLBitmapCollection.hpp"

class GLBitmapFont : public GLBitmapCollection {
public:
    GLBitmapFont(void) :
        GLBitmapCollection(),
        _totalHeight(0) {
        for (int i = 0; i < 256; i++) {
            _charInfo[i] = ~0;
        }
    }

    virtual ~GLBitmapFont() {}

    //Draw a string at (x,y) scaled by [scalex,scaley]
    void DrawString(const char* s, float x, float y, float scalex, float scaley);

    //Determine width of string (doesn't take TABs into account, yet!)
    float GetWidth(const char* s, float scalex);

    float GetHeight(float scaley);

    //Load bitmap and data files for font
    virtual bool Load(const char* bitmapFile, const char* dataFile);

private:
    GLBitmapFont(const GLBitmapFont&);
    GLBitmapFont& operator=(const GLBitmapFont&);

    int _totalHeight;
    unsigned int _charInfo[256];
};
