#pragma once
// Description:
//   Quaternions
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

#ifdef WIN32
#define _USE_MATH_DEFINES  //needed to get M_PI
#endif
#include <math.h>

#include "Point.hpp"

const double EPSILON = 0.0001;
const double TO_RADIANS = M_PI / 180.0;
const double TO_DEGREES = 180.0 / M_PI;

struct Quaternion {
    double w;
    double x;
    double y;
    double z;

    Quaternion() :
        w(1.0),
        x(0.0),
        y(0.0),
        z(0.0) {}

    Quaternion(double W, double X, double Y, double Z) :
        w(W),
        x(X),
        y(Y),
        z(Z) {}

    Quaternion(double angle, const Point3D& axis) :
        w(1.0),
        x(0.0),
        y(0.0),
        z(0.0) {
        set(angle, axis);
    }

    Quaternion(const Quaternion& q) :
        w(q.w),
        x(q.x),
        y(q.y),
        z(q.z) {}

    Quaternion& operator=(const Quaternion& q) {
        w = q.w;
        x = q.x;
        y = q.y;
        z = q.z;
        return *this;
    }

    void identity(void) {
        w = 1.0;
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    void norm() {
        double lensqr = w * w + x * x + y * y + z * z;
        if (fabs(lensqr - 1.0) > EPSILON) {
            if (lensqr < (EPSILON * EPSILON)) {
                identity();
                return;
            }

            double len = sqrt(lensqr);
            w = w / len;
            x = x / len;
            y = y / len;
            z = z / len;
        }
    }

    //angle in degrees, axis must be normalized
    void set(double angle, const Point3D& axis) {
        double halfAngle = angle * 0.5 * TO_RADIANS;
        double sina = sin(halfAngle);
        double cosa = cos(halfAngle);

        x = axis.x * sina;
        y = axis.y * sina;
        z = axis.z * sina;
        w = cosa;
    }

    //angle in degrees
    void get(float& angle, Point3D& axis) {
        double cosa = w;
        //cos^2 + sin^2 = 1 -> sin = sqrt(1 - cos^2)
        //The sqrt(1.0-cos^2) can get nasty if cos^2 is a bit bigger then 1!
        //double sina  = sqrt( 1.0 - cosa * cosa );

        //This is another way of calculating the sina. We know this one will
        //be positive so it's safer than the above.
        double sina = sqrt(x * x + y * y + z * z);

        if (fabs(sina) < EPSILON) {
            axis.x = 1.0;
            axis.y = 0.0;
            axis.z = 0.0;
            angle = 0.0;
        } else {
            axis.x = (float)(x / sina);
            axis.y = (float)(y / sina);
            axis.z = (float)(z / sina);
            angle = (float)(acos(cosa) * 2.0 * TO_DEGREES);
        }
    }
};

inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
    // * is multiply, . is dot product, x is cross product, +/- take a guess
    //
    // q1 * q2 =( w1*w2 - v1.v2 , w1.v2 + w2.v1 + v1 x v2)
    //
    // w = w1*w2 - (x1*x2 + y1*y2 + z1*z2)
    // x = w1*x2 + w2*x1 + (y1*z2 - z1*y2)
    // y = w1*y2 + w2*y1 + (z1*x2 - x1*z2)
    // z = w1*z2 + w2*z1 + (x1*y2 - y1*x2)

    return Quaternion(
        // rearranged to align q1.wxyz and to confuse the innocent
        q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z,
        q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y,
        q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x,
        q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w);
}

#if 0
#include <Trace.hpp>
inline void dumpQuaternion( const char *txt, const Quaternion &q)
{
    std::cout.precision(20);
    LOG_INFO << txt << ": "
        << q.w << " "
        << q.x << " "
        << q.y << " "
        << q.z << "\n";
}
#endif
