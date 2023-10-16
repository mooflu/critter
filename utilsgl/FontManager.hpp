#pragma once
// Description:
//   Font Manager.
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

#include "ResourceCache.hpp"
#include "Singleton.hpp"
#include "GLBitmapFont.hpp"

class FontManager : public ResourceCache<GLBitmapFont> {
    friend class Singleton<FontManager>;

public:
    GLBitmapFont* getFont(const std::string& fontName) { return getResource(fontName); }

    virtual void reload(void);
    virtual void reset(void);

protected:
    virtual GLBitmapFont* load(const std::string& font);

private:
    FontManager(void);
    virtual ~FontManager();
    FontManager(const FontManager&);
    FontManager& operator=(const FontManager&);
};

typedef Singleton<FontManager> FontManagerS;
