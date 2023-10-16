// Description:
//   Wrapper for a GL texture in conjunction with SDL.
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
#include "GLTexture.hpp"

#include "Trace.hpp"
#include "TextureManager.hpp"

//Construct texture given filename
GLTexture::GLTexture(GLenum target, const char* fileName, bool mipmap) :
    _target(target) {
    XTRACE();
    SDL_Surface* image;
    if ((image = IMG_Load(fileName)) == 0) {
        LOG_ERROR << "Couldn't load " << fileName << ": " << SDL_GetError() << "\n";
        _textureID = 0;
        _image = 0;
    } else {
        init(image, mipmap);
    }
}

//Construct texture given SDL surface
GLTexture::GLTexture(GLenum target, SDL_Surface* img, bool mipmap) :
    _target(target) {
    XTRACE();
    init(img, mipmap);
}

GLTexture::~GLTexture() {
    XTRACE();
    TextureManagerS::instance()->removeTexture(this);
    if (_image) {
        SDL_FreeSurface(_image);
    }
}

void GLTexture::reset(void) {
    TextureManagerS::instance()->removeTexture(this);
}

void GLTexture::reload(void) {
    init(_image, _mipmap);
}

//Init texture with SDL surface
void GLTexture::init(SDL_Surface* img, bool mipmap) {
    _image = img;
    _mipmap = mipmap;
    _textureID = TextureManagerS::instance()->addTexture(this);

    bind();
    glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (mipmap) {
#ifdef __APPLE__
#ifndef IPHONE
        //TODO: check for extension
        glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
        glTexParameteri(_target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
#endif
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexImage2D(_target, 0, GL_RGBA, _image->w, _image->h, 0, getGLTextureFormat(), GL_UNSIGNED_BYTE,
                     _image->pixels);
#else
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
        glTexParameteri(_target, GL_GENERATE_MIPMAP, GL_TRUE);

        //glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
        //glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP);

        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexImage2D(_target, 0, GL_RGBA8, _image->w, _image->h, 0, getGLTextureFormat(), GL_UNSIGNED_BYTE,
                     _image->pixels);

        //Doesn't work???
        //glEnable(_target);
        //glGenerateMipmap(_target);
#endif
    } else {
        glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifdef IPHONE
        if (_image->compressedSize > 0) {
            glCompressedTexImage2D(_target, 0, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, _image->w, _image->h, 0,
                                   _image->compressedSize, _image->pixels);
        } else
#endif
        {
#ifdef USE_16BIT_TEXTURE
            int pixelCount = _image->w * _image->h;
            unsigned short* outPixel16 = (unsigned short*)malloc(pixelCount * 2);
            unsigned short* pixels = outPixel16;
            unsigned int* inPixel32 = (unsigned int*)_image->pixels;
            for (int i = 0; i < pixelCount; i++) {
                *outPixel16 =
                    ((((*inPixel32 >>  4) & 0xF)) << 12) |
                    ((((*inPixel32 >> 12) & 0xF)) << 8) |
                    ((((*inPixel32 >> 20) & 0xF)) << 4) |
                    ((((*inPixel32 >> 28) & 0xF)) << 0);
                outPixel16++;
                inPixel32++;
            }
            glTexImage2D(_target, 0, GL_RGBA, _image->w, _image->h, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pixels);
            free(pixels);
#else
            //LOG_INFO << "IMG " << SDL_GetPixelFormatName(_image->format->format) << "\n";
            // reported as "SDL_PIXELFORMAT_ARGB8888"
            glTexImage2D(_target, 0, GL_RGBA, _image->w, _image->h, 0, getGLTextureFormat(), GL_UNSIGNED_BYTE,
                         _image->pixels);
#endif
        }
    }
}

//get texture format
GLenum GLTexture::getGLTextureFormat(void) {
    int texFormat = GL_RGB;

    Uint8 alpha = 0;
    SDL_GetSurfaceAlphaMod(_image, &alpha);

    if (alpha > 0) {
        texFormat = GL_RGBA;
    }

    return texFormat;
}
