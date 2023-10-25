// Description:
//   Wrapper for a GL texture cube map in conjunction with SDL.
//
// Copyright (C) 2005 Frank Becker
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
#include "GLTextureCubeMap.hpp"

#include "Trace.hpp"
#include "TextureManager.hpp"

static GLenum cubeMapTarget[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB};

GLTextureCubeMap::GLTextureCubeMap(SDL_Surface* image[6], bool mipmap) :
    _mipmap(mipmap) {
    for (int i = 0; i < 6; i++) {
        _image[i] = image[i];
    }
    init();
}

GLTextureCubeMap::~GLTextureCubeMap() {
    TextureManagerS::instance()->removeTexture(this);
    for (int i = 0; i < 6; i++) {
        if (_image[i]) {
            SDL_FreeSurface(_image[i]);
        }
    }
}

GLenum GLTextureCubeMap::getGLTextureFormat(SDL_Surface* image) {
    int texFormat = GL_RGB;

    Uint8 alpha = 0;
    SDL_GetSurfaceAlphaMod(image, &alpha);

    if (alpha > 0) {
        texFormat = GL_RGBA;
    }

    return texFormat;
}

void GLTextureCubeMap::init(void) {
    _textureID = TextureManagerS::instance()->addTexture(this);
    bind();

    //    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    for (int i = 0; i < 6; i++) {
        SDL_Surface* image = _image[i];
        GLenum target = cubeMapTarget[i];

        if (_mipmap) {
#if 0
	    __gluBuild2DMipmaps( target, GL_RGBA8, image->w, image->h,
		    getGLTextureFormat(image), GL_UNSIGNED_BYTE, image->pixels);
#else
            LOG_ERROR << "midmap needs work\n";
#endif
        } else {
            glTexImage2D(target, 0, GL_RGBA8, image->w, image->h, 0, getGLTextureFormat(image), GL_UNSIGNED_BYTE,
                         image->pixels);
        }
    }

    if (_mipmap) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

#if 0
    int mode = GL_NORMAL_MAP_ARB;  //GL_REFLECTION_MAP_ARB; //GL_NORMAL_MAP_ARB
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);

    int wrap = GL_CLAMP;  //GL_REPEAT; //GL_CLAMP;
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, wrap);
#endif
}

void GLTextureCubeMap::reload(void) {
    TextureManagerS::instance()->removeTexture(this);
    init();
}
