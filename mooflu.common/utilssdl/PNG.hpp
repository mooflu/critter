#pragma once
// Description:
//   Helper to save a SDL surface as a PNG.
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
#include <png.h>
#include "SDL.h"

class PNG {
public:
    //Contruct with or without alpha
    PNG(bool alpha = true) :
        _alpha(alpha) {}

    //Quick way to save snapshot (no alpha)
    static bool Snapshot(SDL_Surface* img, const std::string& filename) {
        PNG png(false);
        return png.Save(img, filename);
    }

    //Save SDL surface as png
    bool Save(SDL_Surface* img, const std::string& filename, bool flip = false);

private:
    bool _alpha;
    png_structp _png;
    png_infop _info;

    bool init(FILE* fp, int width, int height);
    void close(FILE* fp);
    static void writeData(png_structp png, png_bytep data, png_size_t length);
};
