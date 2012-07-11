#pragma once
// Description:
//   Manager for GLTextures.
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
#include "Singleton.hpp"

#define MAX_TEXTURES 1024
class TextureManager
{
friend class Singleton<TextureManager>;
public:
    int addTexture( GLTextureI *tex);
    void removeTexture( GLTextureI *tex);

protected:
    TextureManager( void);
    ~TextureManager();

    int findTexture( GLTextureI *tex);
    GLTextureI *texArray[ MAX_TEXTURES];
    GLuint     texIDs[ MAX_TEXTURES];
private:
    int _textureCount;
};

typedef Singleton<TextureManager> TextureManagerS;
