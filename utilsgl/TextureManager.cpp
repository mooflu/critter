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
#include "TextureManager.hpp"

#include "stdio.h"
#include "Trace.hpp"

TextureManager::TextureManager(void) :
    _textureCount(0) {
    XTRACE();
    for (int i = 1; i < MAX_TEXTURES; i++) {
        texArray[i] = 0;
    }
}

TextureManager::~TextureManager() {
    XTRACE();
    //We don't delete GLTextures here. We keep
    //track of textures, but don't own them.
}

int TextureManager::addTexture(GLTextureI* tex) {
    int slot = findTexture(0);
    if (slot > 0) {
        texArray[slot] = tex;
        glGenTextures(1, &(texIDs[slot]));
        _textureCount++;
    } else {
        fprintf(stderr, "TextureManager is full!\n");
    }
    return texIDs[slot];
}

void TextureManager::removeTexture(GLTextureI* tex) {
    int slot = findTexture(tex);
    if (slot > 0) {
        texArray[slot] = 0;
        glDeleteTextures(1, &(texIDs[slot]));
        _textureCount--;
    }

    if (_textureCount == 0) {
        TextureManagerS::cleanup();
    }
}

int TextureManager::findTexture(GLTextureI* tex) {
    for (int i = 1; i < MAX_TEXTURES; i++) {
        if (texArray[i] == tex) {
            return i;
        }
    }
    return -1;
}
