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
#include "PNG.hpp"

//Save SDL surface as png
bool PNG::Save(SDL_Surface* img, const std::string& filename, bool flip) {
    FILE* fp = fopen(filename.c_str(), "wb");
    if (fp == NULL) {
        return false;
    }

    if (!init(fp, img->w, img->h)) {
        return false;
    }

    unsigned char* data = (unsigned char*)img->pixels;
    if (flip) {
        for (int y = 0; y < img->h; y++) {
            png_bytep rowPointers = &data[img->pitch * y];
            png_write_rows(_png, &rowPointers, 1);
        }
    } else {
        for (int y = img->h - 1; y >= 0; y--) {
            png_bytep rowPointers = &data[img->pitch * y];
            png_write_rows(_png, &rowPointers, 1);
        }
    }

    close(fp);

    return true;
}

//write a chunk of png data
void PNG::writeData(png_structp png, png_bytep data, png_size_t length) {
    png_size_t check;

    check = fwrite(data, 1, length, (FILE*)png_get_io_ptr(png));
    if (check != length) {
        png_error(png, "Write Error");
    }
}

//init with file pointer and size of bitmap
bool PNG::init(FILE* fp, int width, int height) {
    _png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (_png == NULL) {
        fclose(fp);
        return false;
    }

    _info = png_create_info_struct(_png);

    if (_info == NULL) {
        fclose(fp);
        png_destroy_write_struct(&_png, (png_infopp)NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(_png))) {
        fclose(fp);
        png_destroy_write_struct(&_png, (png_infopp)NULL);
        return false;
    }

    png_set_write_fn(_png, (void*)fp, PNG::writeData, NULL);

    int colorType = PNG_COLOR_TYPE_RGB;
    if (_alpha) {
        colorType = PNG_COLOR_TYPE_RGB_ALPHA;
    }

    png_set_IHDR(_png, _info, width, height, 8 /*depth*/, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_write_info(_png, _info);

    return true;
}

//close png file
void PNG::close(FILE* fp) {
    png_write_end(_png, _info);

    png_destroy_write_struct(&_png, (png_infopp)NULL);

    fclose(fp);
}
