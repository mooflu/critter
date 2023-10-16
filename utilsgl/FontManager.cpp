// Description:
//   Font Manager.
//
// Copyright (C) 2003 Frank Becker
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
#include "FontManager.hpp"
#include "Trace.hpp"

using namespace std;

FontManager::FontManager(void) {
    XTRACE();
}

FontManager::~FontManager() {
    XTRACE();
}

void FontManager::reset(void) {
    XTRACE();
    hash_map<const string, GLBitmapFont*, hash<const string>>::const_iterator ci;
    for (ci = _resourceMap.begin(); ci != _resourceMap.end(); ci++) {
        GLBitmapFont* font = ci->second;
        font->reset();
    }
}

void FontManager::reload(void) {
    XTRACE();
    hash_map<const string, GLBitmapFont*, hash<const string>>::const_iterator ci;
    for (ci = _resourceMap.begin(); ci != _resourceMap.end(); ci++) {
        GLBitmapFont* font = ci->second;
        font->reload();
    }
}

GLBitmapFont* FontManager::load(const string& fontName) {
    XTRACE();
    GLBitmapFont* font = new GLBitmapFont();

    string fontPNG = fontName + ".font";
    string fontData = fontName + ".data";

    if (!font->Load(fontPNG.c_str(), fontData.c_str())) {
        LOG_ERROR << "Unable to load font: " << fontName << endl;
        delete font;
        return 0;
    }

    return font;
}
