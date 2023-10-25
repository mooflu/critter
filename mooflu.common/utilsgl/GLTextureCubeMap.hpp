#pragma once
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

#include "SDL2/SDL_image.h"
#include <GL/glew.h>
#include "GLTexture.hpp"

#ifndef GL_ARB_texture_cube_map
#define GL_NORMAL_MAP_ARB                  0x8511
#define GL_REFLECTION_MAP_ARB              0x8512
#define GL_TEXTURE_CUBE_MAP_ARB            0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB    0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB      0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB   0x851C
#endif

class GLTextureCubeMap : public GLTextureI {
public:
    GLTextureCubeMap(SDL_Surface* image[6], bool mipmap = false);

    ~GLTextureCubeMap();

    void bind(void) { glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, _textureID); }

    void unbind(void) { glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0); }

    void reload(void);

private:
    GLuint _textureID;
    SDL_Surface* _image[6];
    bool _mipmap;

    void init(void);
    GLenum getGLTextureFormat(SDL_Surface* image);
};
