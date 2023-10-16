#pragma once
// Description:
//   Bezier segment template.
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

#include "Point.hpp"

const int MAX_DIMENSION = 4;

template <class POINT>
class Bezier {
public:
    //Construct a Bezier segment given 4 points
    Bezier(const POINT p[4]);

    //Calculate point on the Bezier segment at time 0<=t<=1
    void Pos(const float t, POINT& p) const;
    //Calculate perpendicular vector at time 0<=t<=1
    void PVec(const float t, POINT& p) const;
    //Calculate tangent vector at time 0<=t<=1
    void TVec(const float t, POINT& p) const;

private:
    Bezier(void);
    Bezier(const Bezier&);
    Bezier& operator=(const Bezier&);

    void calcCoeff(const float in[4], float out[4]);

    POINT _controlPoints[4];
    float pd[MAX_DIMENSION][4];
};

#include "Bezier.cpp"
