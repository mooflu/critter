#pragma once
// Description:
//   2D and 3D points.
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

#include <cmath>
#include <limits>

template <typename T>
inline void fclamp(T& val, const T& MINval, const T& MAXval) {
    if (val < MINval) {
        val = MINval;
    } else if (val > MAXval) {
        val = MAXval;
    }
}

template <typename T>
inline bool feq(const T& v1, const T& v2, T tolerance = std::numeric_limits<T>::epsilon()) {
    bool ok = true;
    ok = fabs(v1 - v2) < tolerance;
    return ok;
}

template <typename T>
inline T fmin(const T& f1, const T& f2) {
    return f1 < f2 ? f1 : f2;
}

template <typename T>
inline T fmax(const T& f1, const T& f2) {
    return f1 > f2 ? f1 : f2;
}

//-----------------------------------------------------------------------

struct Point2D {
    float x;
    float y;

    Point2D() :
        x(0),
        y(0) {}

    Point2D(float d1, float d2) :
        x(d1),
        y(d2) {}

    static int dimension(void) { return 2; }

    float operator[](int idx) {
        switch (idx) {
            case 0:
                return x;
            case 1:
                return y;

            default:
                return 0;
        }
    }

    void set(int idx, float val) {
        switch (idx) {
            case 0:
                x = val;
                break;
            case 1:
                y = val;
                break;
            default:
                break;
        }
    }

    inline Point2D& operator+=(const Point2D& p) {
        x += p.x;
        y += p.y;

        return *this;
    }

    inline Point2D& operator-=(const Point2D& p) {
        x -= p.x;
        y -= p.y;

        return *this;
    }
};

struct Point2Di {
    Point2Di() :
        x(0),
        y(0) {}

    Point2Di(int d1, int d2) :
        x(d1),
        y(d2) {}

    int x;
    int y;
};

//-----------------------------------------------------------------------

struct Point3Di {
    Point3Di() :
        x(0),
        y(0),
        z(0) {}

    Point3Di(int d1, int d2, int d3) :
        x(d1),
        y(d2),
        z(d3) {}

    int x;
    int y;
    int z;
};

struct Point3D {
    float x;
    float y;
    float z;

    Point3D() :
        x(0),
        y(0),
        z(0) {}

    Point3D(float d1, float d2, float d3) :
        x(d1),
        y(d2),
        z(d3) {}

    static int dimension(void) { return 3; }

    float operator[](int idx) {
        switch (idx) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;

            default:
                return 0;
        }
    }

    void set(int idx, float val) {
        switch (idx) {
            case 0:
                x = val;
                break;
            case 1:
                y = val;
                break;
            case 2:
                z = val;
                break;
            default:
                break;
        }
    }

    inline Point3D& operator=(const Point3Di& point) {
        x = (float)point.x;
        y = (float)point.y;
        z = (float)point.z;
        return *this;
    }
};

//-----------------------------------------------------------------------

inline Point2D operator+(const Point2D& p1, const Point2D p2) {
    return (Point2D(p1.x + p2.x, p1.y + p2.y));
}

inline Point2D operator-(const Point2D& p1, const Point2D& p2) {
    return (Point2D(p1.x - p2.x, p1.y - p2.y));
}

inline Point2D operator/(const Point2D& p1, float d) {
    return (Point2D(p1.x / d, p1.y / d));
}

inline void norm(Point2D& p) {
    float dist = sqrt(p.x * p.x + p.y * p.y);

    p.x = p.x / dist;
    p.y = p.y / dist;
}

inline float dist(const Point2D& p) {
    return sqrt(p.x * p.x + p.y * p.y);
}

inline Point2D operator*(const Point2D& p1, const float s) {
    return (Point2D(p1.x * s, p1.y * s));
}

inline bool operator==(const Point2D& p1, const Point2D& p2) {
    return (feq(p1.x, p2.x) && feq(p1.y, p2.y));
}

inline bool operator!=(const Point2D& p1, const Point2D& p2) {
    return !(p1 == p2);
}

struct BoundingBox {
    BoundingBox() :
        _empty(true) {}

    BoundingBox(const Point2D& p1, const Point2D& p2) :
        _empty(false) {
        min.x = ::fmin(p1.x, p2.x);
        min.y = ::fmin(p1.y, p2.y);
        max.x = ::fmax(p1.x, p2.x);
        max.y = ::fmax(p1.y, p2.y);
    }

    bool containsPoint(const Point2D& p) {
        return (p.x >= min.x) && (p.x <= max.x) && (p.y >= min.y) && (p.y <= max.y);
    }

    void merge(const BoundingBox& bb) {
        if (_empty) {
            min = bb.min;
            max = bb.max;
            _empty = false;
        } else {
            min.x = ::fmin(bb.min.x, min.x);
            min.y = ::fmin(bb.min.y, min.y);
            max.x = ::fmax(bb.max.x, max.x);
            max.y = ::fmax(bb.max.y, max.y);
        }
    }

    Point2D min;
    Point2D max;

private:
    bool _empty;
};

//-----------------------------------------------------------------------

inline Point3D operator+(const Point3D p1, const Point3D p2) {
    return (Point3D(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z));
}

inline Point3D operator-(const Point3D p1, const Point3D p2) {
    return (Point3D(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z));
}

inline Point3D operator*(const Point3D p1, const float s) {
    return (Point3D(p1.x * s, p1.y * s, p1.z * s));
}

inline Point3D operator/(const Point3D p1, float d) {
    return (Point3D(p1.x / d, p1.y / d, p1.z / d));
}

inline bool operator==(const Point3D p1, const Point3D p2) {
    return (feq(p1.x, p2.x) && feq(p1.y, p2.y) && feq(p1.z, p2.z));
}

inline bool operator!=(const Point3D p1, const Point3D p2) {
    return !(p1 == p2);
}

inline float dot(const Point3D p1, const Point3D p2) {
    return (p1.x * p2.x + p1.y * p2.y + p1.z * p2.z);
}

inline Point3D cross(const Point3D p1, const Point3D p2) {
    return (Point3D(p1.y * p2.z - p1.z * p2.y, p1.z * p2.x - p1.x * p2.z, p1.x * p2.y - p1.y * p2.x));
}

inline float dist(const Point3D& p) {
    return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

inline void norm(Point3D& p) {
    float d = dist(p);

    p.x = p.x / d;
    p.y = p.y / d;
    p.z = p.z / d;
}

inline void Clamp(float& val, const float& MINval, const float& MAXval) {
    if (val < MINval) {
        val = MINval;
    } else if (val > MAXval) {
        val = MAXval;
    }
}

//-----------------------------------------------------------------------

inline Point3Di operator+(const Point3Di p1, const Point3Di p2) {
    return (Point3Di(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z));
}

inline Point3Di operator-(const Point3Di p1, const Point3Di p2) {
    return (Point3Di(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z));
}

inline bool operator==(const Point3Di p1, const Point3Di p2) {
    return ((p1.x == p2.x) && (p1.y == p2.y) && (p1.z == p2.z));
}

inline bool operator!=(const Point3Di p1, const Point3Di p2) {
    return !(p1 == p2);
}

//-----------------------------------------------------------------------

struct vec3 {
    float& x;
    float& y;
    float& z;

    float v[3];

    vec3(void) :
        x(v[0]),
        y(v[1]),
        z(v[2]) {}

    vec3(float X, float Y, float Z) :
        x(v[0]),
        y(v[1]),
        z(v[2]) {
        x = X;
        y = Y;
        z = Z;
    }

    vec3(const vec3& vi) :
        x(v[0]),
        y(v[1]),
        z(v[2]) {
        v[0] = vi.x;
        v[1] = vi.y;
        v[2] = vi.z;
    }

    vec3& operator=(const vec3& vi) {
        v[0] = vi.x;
        v[1] = vi.y;
        v[2] = vi.z;

        return *this;
    }
};

inline vec3 operator+(const vec3 p1, const vec3 p2) {
    return (vec3(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z));
}

inline vec3 operator-(const vec3 p1, const vec3 p2) {
    return (vec3(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z));
}

inline vec3 operator*(const vec3 p1, const float s) {
    return (vec3(p1.x * s, p1.y * s, p1.z * s));
}

//-----------------------------------------------------------------------

struct vec4 {
    float& x;
    float& y;
    float& z;
    float& a;

    float v[4];

    vec4(void) :
        x(v[0]),
        y(v[1]),
        z(v[2]),
        a(v[3]) {}

    vec4(float X, float Y, float Z, float A) :
        x(v[0]),
        y(v[1]),
        z(v[2]),
        a(v[3]) {
        x = X;
        y = Y;
        z = Z;
        a = A;
    }

    vec4(const vec4& vi) :
        x(v[0]),
        y(v[1]),
        z(v[2]),
        a(v[3]) {
        v[0] = vi.x;
        v[1] = vi.y;
        v[2] = vi.z;
        v[3] = vi.a;
    }

    vec4& operator=(const vec4& vi) {
        v[0] = vi.x;
        v[1] = vi.y;
        v[2] = vi.z;
        v[3] = vi.a;

        return *this;
    }
};

//-----------------------------------------------------------------------

struct Dimension {
    int width;
    int height;

    Dimension(void) :
        width(0),
        height(0) {}

    Dimension(int w, int h) :
        width(w),
        height(h) {}

    int Area(void) const { return width * height; }

    bool operator<(const Dimension& d) const { return width < d.width || height < d.height; }

    bool operator==(const Dimension& d) const { return d.width == width && d.height == height; }
};
