#pragma once

// Description:
//   RGB Color
//
// Copyright (C) 2008 Frank Becker

struct ColorRGB {
    float& r;
    float& g;
    float& b;

    float v[3];

    ColorRGB(void) :
        r(v[0]),
        g(v[1]),
        b(v[2]) {}

    ColorRGB(double R, double G, double B) :
        r(v[0]),
        g(v[1]),
        b(v[2]) {
        r = R;
        g = G;
        b = B;
    }

    ColorRGB(float R, float G, float B) :
        r(v[0]),
        g(v[1]),
        b(v[2]) {
        r = R;
        g = G;
        b = B;
    }

    ColorRGB(int R, int G, int B) :
        r(v[0]),
        g(v[1]),
        b(v[2]) {
        r = (float)R / 255.0f;
        g = (float)G / 255.0f;
        b = (float)B / 255.0f;
    }

    ColorRGB(const ColorRGB& vi) :
        r(v[0]),
        g(v[1]),
        b(v[2]) {
        v[0] = vi.r;
        v[1] = vi.g;
        v[2] = vi.b;
    }

    ColorRGB& operator=(const ColorRGB& vi) {
        v[0] = vi.r;
        v[1] = vi.g;
        v[2] = vi.b;

        return *this;
    }
};
