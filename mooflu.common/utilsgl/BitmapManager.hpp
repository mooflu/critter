#pragma once
// Description:
//   Bitmap Manager.
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
#include <string>

#include "ResourceCache.hpp"
#include "Singleton.hpp"
#include "GLBitmapCollection.hpp"

class BitmapManager : public ResourceCache<GLBitmapCollection> {
    friend class Singleton<BitmapManager>;

public:
    GLBitmapCollection* getBitmap(const std::string& bitmapName) {
#ifdef IPHONE
        return getResource(std::string("bitmaps/atlas"));
#else
        return getResource(bitmapName);
#endif
    }

    virtual void reload(void);
    virtual void reset(void);

protected:
    virtual GLBitmapCollection* load(const std::string& bitmap);

private:
    BitmapManager(void);
    virtual ~BitmapManager();
    BitmapManager(const BitmapManager&);
    BitmapManager& operator=(const BitmapManager&);
};

typedef Singleton<BitmapManager> BitmapManagerS;
