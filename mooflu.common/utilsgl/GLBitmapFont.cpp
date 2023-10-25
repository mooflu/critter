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
#include "GLBitmapFont.hpp"

#include <stdio.h>
#include "Trace.hpp"
#include "BitmapManager.hpp"

#include "gl3/ProgramManager.hpp"
#include "gl3/Program.hpp"
#include "gl3/Buffer.hpp"
#include "gl3/VertexArray.hpp"

#include <string>
using namespace std;

//Draw a string at (x,y) scaled by [scalex,scaley]
void GLBitmapFont::DrawString(const char* s, float x, float y, float scalex, float scaley) {
    XTRACE();

    unsigned char c;
    int i, l;

    l = (int)strlen(s);
    bind();

    Program* prog = ProgramManagerS::instance()->getProgram("texture");
    prog->use();  //needed to set uniforms

    GLint color = glGetUniformLocation(prog->id(), "aColor");
    glUniform4fv(color, 1, _color.array);

    GLint withTexture = glGetUniformLocation(prog->id(), "withTexture");
    glUniform1i(withTexture, 1);

    _vao->bind();

    for (i = 0; i < l; i++) {
        float dxsize, dysize;
        c = s[i];
        if (_charInfo[c] == ((unsigned int)~0)) {
            //no bitmap for char
            continue;
        }
        BitmapInfo& charInfo = _bitmapInfo[_charInfo[c]];

        if (c == 32) {
            dxsize = (float)charInfo.width * scalex;
        } else if (c == '\t') {
            int tabwidth = (int)((float)(_bitmapInfo[_charInfo[32]].width * 8) * scalex);
            int xpos = (int)x;
            int trunc = xpos - (xpos % tabwidth);
            x = (float)(trunc + tabwidth);
            continue;
        } else {  // don't do space
            float tx, ty, fxsize, fysize, ay;

            dxsize = (float)charInfo.width * scalex;
            dysize = (float)charInfo.height * scaley;
            ay = (float)(_totalHeight - charInfo.yoff) * scaley;
            fxsize = (float)charInfo.width / _textureSize;
            fysize = (float)charInfo.height / _textureSize;
            tx = (float)charInfo.xpos / _textureSize;
            ty = (float)charInfo.ypos / _textureSize;

            GLfloat squareVertices[] = {
                x       ,y+ay,          0,
                x+dxsize,y+ay,          0,
                x+dxsize,y+ay-dysize,   0,
                x       ,y+ay-dysize,   0,
            };

            GLfloat squareTexCoords[] = {
                tx       ,ty,
                tx+fxsize,ty,
                tx+fxsize,ty+fysize,
                tx       ,ty+fysize
            };

            //glVertexPointer(2, GL_FLOAT, 0, squareVertices);
            //glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);

            _vertBuf->bind(GL_ARRAY_BUFFER);
            _vertBuf->setData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), squareVertices, GL_STATIC_DRAW);

            _texCoordBuf->bind(GL_ARRAY_BUFFER);
            _texCoordBuf->setData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), squareTexCoords, GL_STATIC_DRAW);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        x += dxsize;  //charInfo.width;
    }

    _vao->unbind();

    prog->release();
}

//Determine width of string (doesn't take TABs into account, yet!)
float GLBitmapFont::GetWidth(const char* s, float scalex) {
    XTRACE();
    int width = 0;
    size_t l = strlen(s);
    for (size_t i = 0; i < l; i++) {
        //FIXME: handle TABs
        width += _bitmapInfo[_charInfo[(int)s[i]]].width;
    }
    return ((float)width * scalex);
}

float GLBitmapFont::GetHeight(float scaley) {
    XTRACE();
    return (float)_totalHeight * scaley;
}

//Load bitmap and data files for font
bool GLBitmapFont::Load(const char* bitmapFile, const char* dataFile) {
    XTRACE();
#ifdef IPHONE
    GLBitmapCollection* bitmaps = BitmapManagerS::instance()->getBitmap("bitmaps/atlas");
    bool result = (bitmaps != 0);

    _bitmapCollection = bitmaps->_bitmapCollection;
    _bcNeedsCleanup = false;
    for (unsigned int i = 0; i < bitmaps->_bitmapCount; i++) {
        _bitmapInfo[i] = bitmaps->_bitmapInfo[i];
    }
    _bitmapCount = bitmaps->_bitmapCount;
    _textureSize = bitmaps->_textureSize;

    string bitmapFileName(bitmapFile);
    size_t epos = bitmapFileName.find_last_of('.');
    size_t spos = bitmapFileName.find_last_of('/');
    string baseName = "not found";
    if (epos != string::npos) {
        baseName = bitmapFileName.substr(spos + 1, epos - (spos + 1));
    }
#else
    bool result = GLBitmapCollection::Load(bitmapFile, dataFile);
#endif
    if (!result) {
        LOG_ERROR << "Unable to load font...\n";
    }

    _totalHeight = 0;
    for (unsigned int i = 0; i < _bitmapCount; i++) {
        string name = _bitmapInfo[i].name;
#ifdef IPHONE
        if (name.find(baseName) != 0) {
            continue;
        }
#endif
        int theChar = (int)_bitmapInfo[i].name[name.length() - 1];
        //calc _totalHeight
        int height = _bitmapInfo[i].yoff + _bitmapInfo[i].height;

        if (height > _totalHeight) {
            _totalHeight = height;
        }

        //provide mapping for char -> _bitmapInfo index
        _charInfo[theChar] = i;
    }
    //add a little extra space...
    _totalHeight += 2;

    return result;
}
