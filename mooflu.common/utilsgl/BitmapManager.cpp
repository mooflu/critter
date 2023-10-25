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
#include "BitmapManager.hpp"
#include "Trace.hpp"

using namespace std;

BitmapManager::BitmapManager(void) {
    XTRACE();
}

BitmapManager::~BitmapManager() {
    XTRACE();
}

void BitmapManager::reset(void) {
    XTRACE();
    hash_map<const string, GLBitmapCollection*, hash<const string>>::const_iterator ci;
    for (ci = _resourceMap.begin(); ci != _resourceMap.end(); ci++) {
        GLBitmapCollection* bitmap = ci->second;
        bitmap->reset();
    }
}

void BitmapManager::reload(void) {
    XTRACE();
    hash_map<const string, GLBitmapCollection*, hash<const string>>::const_iterator ci;
    for (ci = _resourceMap.begin(); ci != _resourceMap.end(); ci++) {
        GLBitmapCollection* bitmap = ci->second;
        bitmap->reload();
    }
}

GLBitmapCollection* BitmapManager::load(const string& bitmapName) {
    XTRACE();
    GLBitmapCollection* bitmap = new GLBitmapCollection();

    string bitmapData = bitmapName + ".data";

    if (!bitmap->Load(bitmapName.c_str(), bitmapData.c_str())) {
        LOG_ERROR << "Unable to load bitmap collection: " << bitmapName << endl;
        delete bitmap;
        bitmap = 0;
    }

    return bitmap;
}
