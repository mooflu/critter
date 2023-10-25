// Description:
//   A Bitmap Collection contains many bitmaps in a single PNG file.
//   Bitmap names, offsets, sizes, etc. are kept in a seperate data file.
//   Includes GL drawing methods.
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

#include "SDL_image.h"
#include <zrwops.hpp>

#include "Trace.hpp"
#include "FindHash.hpp"
#include "ResourceManager.hpp"

#include "gl3/ProgramManager.hpp"
#include "gl3/Program.hpp"
#include "gl3/Buffer.hpp"
#include "gl3/VertexArray.hpp"

#include <memory>
using namespace std;

GLBitmapCollection::GLBitmapCollection(void) :
    _bitmapCollection(0),
    _bcNeedsCleanup(true),
    _bitmapCount(0),
    _textureSize(0.0),
    _bitmapInfoMap(),
    _color(1, 1, 1, 1),
    _vao(0),
    _vIndexBuf(0),
    _vertBuf(0),
    _texCoordBuf(0) {}

GLBitmapCollection::~GLBitmapCollection() {
    //Note: A bit of a hack for iphone where all bitmap collections are merged into a single collection
    if (_bcNeedsCleanup) {
        delete _bitmapCollection;
    }
    resetVAO();
}

void GLBitmapCollection::setColor(const vec4f& color) {
    _color = color;
}

void GLBitmapCollection::setColor(float r, float g, float b, float a) {
    _color = vec4f(r, g, b, a);
}

//Load bitmap
bool GLBitmapCollection::LoadBitmapFile(const char* bitmapFile) {
    XTRACE();
    SDL_Surface* img;

    string bmName;
    if (ResourceManagerS::instance()->hasResource(string(bitmapFile) + ".png")) {
        bmName = string(bitmapFile) + ".png";
    } else if (ResourceManagerS::instance()->hasResource(string(bitmapFile))) {
        bmName = string(bitmapFile);
    }

    if (bmName != "") {
        std::shared_ptr<ziStream> bminfilePtr(ResourceManagerS::instance()->getInputStream(bmName));
        ziStream& bminfile = *bminfilePtr;

        SDL_RWops* src = RWops_from_ziStream(bminfile);
        img = IMG_LoadPNG_RW(src);
        if (!img) {
            LOG_ERROR << "Failed to load PNG image: [" << bmName << "]" << endl;
            return false;
        }
        SDL_FreeRW(src);
    }
#ifdef IPHONE
    else if (ResourceManagerS::instance()->hasResource(string(bitmapFile) + ".pvr")) {
        // pvr generated from png:
        // /Developer/Platforms/iPhoneOS.platform/usr/bin/texturetool -e PVRTC --bits-per-pixel-4 -p test.png -o a.pvr atlas.png
        int compressedSize = ResourceManagerS::instance()->getResourceSize(string(bitmapFile) + ".pvr");

        img = (SDL_Surface*)malloc(sizeof(SDL_Surface));

        img->w = 1024;
        img->h = 1024;
        img->flags = SDL_SWSURFACE;
        img->flags |= SDL_SRCALPHA;
        img->pitch = 0;
        img->compressedSize = compressedSize;
        img->pixels = (char*)malloc(compressedSize);

        int r = ResourceManagerS::instance()->readResource(img->pixels, compressedSize);
        if (r != compressedSize) {
            LOG_ERROR << "Failed to load PVR image: [" << bitmapFile << "]" << endl;
            free(img->pixels);
            free(img);
            return false;
        }
    }
#endif
    else {
        LOG_WARNING << "No Bitmap file [" << bitmapFile << "] not found." << endl;
        return false;
    }
    LOG_DEBUG << "Bitmap loaded." << endl;

    //assuming texture is square
    _textureSize = (float)img->w;

    _bitmapCollection = new GLTexture(GL_TEXTURE_2D, img, false);

    initVAO();

    return true;
}

void GLBitmapCollection::resetVAO() {
    delete _vao;
    delete _vIndexBuf;
    delete _vertBuf;
    delete _texCoordBuf;
}

void GLBitmapCollection::initVAO() {
    GLuint vi[4] = {0, 1, 2, 3};
    _vIndexBuf = new Buffer();
    _vIndexBuf->bind(GL_ELEMENT_ARRAY_BUFFER);
    _vIndexBuf->setData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), vi, GL_STATIC_DRAW);

    _vertBuf = new Buffer();
    _texCoordBuf = new Buffer();

    Program* prog = ProgramManagerS::instance()->getProgram("texture");
    prog->use();

    GLint vertLoc = 0;  //glGetAttribLocation( prog->id(), "vertex");
    GLint uvLoc = 1;    //glGetAttribLocation( prog->id(), "uv");

    GLint textureUnit = glGetUniformLocation(prog->id(), "textureUnit");

    _vao = new VertexArray();
    _vao->bind();

    glUniform1i(textureUnit, 0);

    glEnableVertexAttribArray(vertLoc);
    _vertBuf->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(uvLoc);
    _texCoordBuf->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    _vIndexBuf->bind(GL_ELEMENT_ARRAY_BUFFER);
    _vao->unbind();
    prog->release();
}

//Load single bitmap
bool GLBitmapCollection::Load(const char* bitmapFile) {
    if (!LoadBitmapFile(bitmapFile)) {
        return false;
    }
    BitmapInfo bInfo;
    bInfo.width = _bitmapCollection->width();
    bInfo.height = _bitmapCollection->height();
    bInfo.xoff = 0;
    bInfo.yoff = 0;
    bInfo.xpos = 0;
    bInfo.ypos = 0;
    bInfo.name[0] = '\0';

    _bitmapInfo[_bitmapCount] = bInfo;
    string tmpName = bInfo.name;
    LOG_DEBUG << "New bitmap: [" << tmpName << "] " << endl;
    _bitmapInfoMap[tmpName] = &_bitmapInfo[_bitmapCount];
    _bitmapCount++;

    return true;
}

//Load bitmap and data file
bool GLBitmapCollection::Load(const char* bitmapFile, const char* dataFile) {
    if (!LoadBitmapFile(bitmapFile)) {
        return false;
    }

    if (!ResourceManagerS::instance()->hasResource(string(dataFile))) {
        LOG_WARNING << "Bitmap data file [" << dataFile << "] not found." << endl;
        if (_bcNeedsCleanup) {
            delete _bitmapCollection;
        }
        return false;
    }
    std::shared_ptr<ziStream> datainfilePtr(ResourceManagerS::instance()->getInputStream(string(dataFile)));
    ziStream& datainfile = *datainfilePtr;

    LOG_DEBUG << "Reading: [" << dataFile << "]." << endl;
    _bitmapCount = 0;
    bool fileRead = false;
    while (!fileRead) {
        BitmapInfo tmpBInfo;
        string line;
        if (getline(datainfile, line).eof()) {
            break;
        }

        int n = sscanf( line.c_str(), "%d %d %d %d %d %d [%[\t-~]31s]\n",
            &tmpBInfo.xoff,
            &tmpBInfo.yoff,
            &tmpBInfo.xpos,
            &tmpBInfo.ypos,
            &tmpBInfo.width,
            &tmpBInfo.height,
            tmpBInfo.name);
        if (tmpBInfo.name[strlen(tmpBInfo.name) - 1] == ']') {
            tmpBInfo.name[strlen(tmpBInfo.name) - 1] = '\0';
        }
        LOG_DEBUG << "Read: [" << tmpBInfo.name << "] " << n << endl;
        if (n == 6) {
            //assume missing element is a space
            strcpy(tmpBInfo.name, " ");
            n++;
        }
        if (n != 7) {
            LOG_WARNING << "Data file [" << dataFile << "] has incorrect format." << endl;
            break;
        } else {
            _bitmapInfo[_bitmapCount] = tmpBInfo;
            string tmpName = tmpBInfo.name;
            LOG_DEBUG << "New bitmap: [" << tmpName << "] " << endl;
            _bitmapInfoMap[tmpName] = &_bitmapInfo[_bitmapCount];
            _bitmapCount++;
        }

        if (_bitmapCount > MAX_BITMAPS) {
            LOG_ERROR << "Maximum number (" << MAX_BITMAPS << ") of bitmaps per collection exceeded." << endl;
            fileRead = true;
        }
    }

    LOG_DEBUG << "Bitmap read OK." << endl;

    return true;
}

//Get index of bitmap with the given name
int GLBitmapCollection::getIndex(const string& name) {
    XTRACE();
    for (unsigned int i = 0; i < _bitmapCount; i++) {
        if (name == _bitmapInfo[i].name) {
            return (int)i;
        }
    }

    return -1;
}

//Draw using bitmap index
void GLBitmapCollection::Draw(const unsigned int index, const float& x, const float& y, const float& scalex,
                              const float& scaley, const float& z) {
    if (index >= _bitmapCount) {
        return;
    }

    _Draw(_bitmapInfo[index], x, y, z, scalex, scaley);
}

//Draw using bitmap name
void GLBitmapCollection::Draw(const string& name, const float& x, const float& y, const float& scalex,
                              const float& scaley, const float& z) {
    //    cout << "Drawing bitmap " << name << endl;
    BitmapInfo* bmInfo = findHash<const string>(name, _bitmapInfoMap);

    if (!bmInfo) {
        cerr << "Unable to fill GLBitmapCollection::Draw request for " << name << endl;
        return;
    }

    _Draw(*bmInfo, x, y, z, scalex, scaley);
}

void GLBitmapCollection::Draw(unsigned int index, const vec2f& ll, const vec2f& ur) {
    const BitmapInfo& bitmapInfo = _bitmapInfo[index];

    float tx, ty, fxsize, fysize;

    fxsize = (float)bitmapInfo.width / _textureSize;
    fysize = (float)bitmapInfo.height / _textureSize;
    tx = (float)bitmapInfo.xpos / _textureSize;
    ty = (float)bitmapInfo.ypos / _textureSize;

    GLfloat z = 0.0;
    GLfloat squareVertices[] = {
        ll.x(), ll.y(), z, ur.x(), ll.y(), z, ur.x(), ur.y(), z, ll.x(), ur.y(), z,
    };

    GLfloat squareTexCoords[] = {tx, ty + fysize, tx + fxsize, ty + fysize, tx + fxsize, ty, tx, ty};

    _Draw(squareVertices, squareTexCoords);
}

//Draw using bitmap info
void GLBitmapCollection::_Draw(const BitmapInfo& bitmapInfo, const float& x, const float& y, const float& z,
                               const float& scalex, const float& scaley) {
    XTRACE();

    float tx, ty, fxsize, fysize, ay;
    float dxsize, dysize;

    dxsize = (float)bitmapInfo.width * scalex;
    dysize = (float)bitmapInfo.height * scaley;
    ay = (float)(bitmapInfo.height - bitmapInfo.yoff) * scaley;
    fxsize = (float)bitmapInfo.width / _textureSize;
    fysize = (float)bitmapInfo.height / _textureSize;
    tx = (float)bitmapInfo.xpos / _textureSize;
    ty = (float)bitmapInfo.ypos / _textureSize;

    GLfloat squareVertices[] = {
        x       ,y+ay       ,z,
        x+dxsize,y+ay       ,z,
        x+dxsize,y+ay-dysize,z,
        x       ,y+ay-dysize,z
    };

    GLfloat squareTexCoords[] = {
        tx       ,ty,
        tx+fxsize,ty,
        tx+fxsize,ty+fysize,
        tx       ,ty+fysize
    };

    _Draw(squareVertices, squareTexCoords);
}

//Draw centered using bitmap index
void GLBitmapCollection::DrawC(const unsigned int index, const float& x, const float& y, const float& scalex,
                               const float& scaley, const float& z) {
    if (index >= _bitmapCount) {
        return;
    }

    _DrawC(_bitmapInfo[index], x, y, z, scalex, scaley);
}

//Draw centered using bitmap name
void GLBitmapCollection::DrawC(const string& name, const float& x, const float& y, const float& scalex,
                               const float& scaley, const float& z) {
    //    cout << "Drawing bitmap " << name << endl;
    BitmapInfo* bmInfo = findHash<const string>(name, _bitmapInfoMap);

    if (!bmInfo) {
        cerr << "Unable to fill GLBitmapCollection::Draw request for " << name << endl;
        return;
    }

    _DrawC(*bmInfo, x, y, z, scalex, scaley);
}

//Draw centered using bitmap info
void GLBitmapCollection::_DrawC(const BitmapInfo& bitmapInfo, const float& x, const float& y, const float& z,
                                const float& scalex, const float& scaley) {
    XTRACE();

    float tx, ty, fxsize, fysize;
    float dxsize, dysize;

    dxsize = (float)bitmapInfo.width * scalex * 0.5f;
    dysize = (float)bitmapInfo.height * scaley * 0.5f;
    fxsize = (float)bitmapInfo.width / _textureSize;
    fysize = (float)bitmapInfo.height / _textureSize;
    tx = (float)bitmapInfo.xpos / _textureSize;
    ty = (float)bitmapInfo.ypos / _textureSize;

    GLfloat squareVertices[] = {
        x-dxsize,y+dysize,z,
        x+dxsize,y+dysize,z,
        x+dxsize,y-dysize,z,
        x-dxsize,y-dysize,z
    };

    GLfloat squareTexCoords[] = {
        tx       ,ty,
        tx+fxsize,ty,
        tx+fxsize,ty+fysize,
        tx       ,ty+fysize
    };

    _Draw(squareVertices, squareTexCoords);
}

void GLBitmapCollection::_Draw(GLfloat squareVertices[], GLfloat squareTexCoords[]) {
    bind();

    Program* prog = ProgramManagerS::instance()->getProgram("texture");
    prog->use();  //needed to set uniforms

    GLint color = glGetUniformLocation(prog->id(), "aColor");
    glUniform4fv(color, 1, _color.array);

    GLint withTexture = glGetUniformLocation(prog->id(), "withTexture");
    glUniform1i(withTexture, 1);

    _vao->bind();
    _vertBuf->bind(GL_ARRAY_BUFFER);
    _vertBuf->setData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), squareVertices, GL_STATIC_DRAW);

    _texCoordBuf->bind(GL_ARRAY_BUFFER);
    _texCoordBuf->setData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), squareTexCoords, GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    _vao->unbind();

    prog->release();
}
