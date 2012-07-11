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
#include "Trace.hpp"
#include "Point.hpp"

//Construct a Bezier segment given 4 points
template<class POINT>
Bezier<POINT>::Bezier( const POINT point[4])
{
    XTRACE();
    int dim = point[0].dimension();
    
    for( int c=0; c<4; c++)
    {
        _controlPoints[c] = point[c];
    }

    for( int d=0; d<dim; d++)
    {
        float val[4];

	for( int i=0; i<4; i++)
	{
            POINT pt = point[ i];
	    val[ i] = pt[d];
	}
	calcCoeff( val, pd[d]);
    }
}

//Calculate Bezier segment coefficients
template<class POINT>
void Bezier<POINT>::calcCoeff( const float in[ 4], float out[ 4])
{
    XTRACE();
    out[ 0] =  1*in[0];
    out[ 1] = -3*in[0] +3*in[1];
    out[ 2] =  3*in[0] -6*in[1] +3*in[2];
    out[ 3] = -1*in[0] +3*in[1] -3*in[2] +1*in[3];
}

//Calculate point on the Bezier segment at time 0<=t<=1
template<class POINT>
void Bezier<POINT>::Pos( const float t, POINT &point) const
{
    XTRACE();
    float tt = t*t;
    float ttt = t*tt;

    for( int d=0; d<point.dimension(); d++)
    {
	point.set(d, pd[d][0] + t*pd[d][1] + tt*pd[d][2] + ttt*pd[d][3]);
    }
}

//Calculate perpendicular vector at time 0<=t<=1
//Really only makes sense in 2D
template<class POINT>
void Bezier<POINT>::PVec( const float t, POINT &point) const
{
    XTRACE();
    float t2 = 2*t;
    float tt3 = 3*t*t;

    if( point.dimension() != 2) return;

    //vector perpendicular to tangent at t
    // x/y -> -(y/x)
    point.set(1,  pd[0][1] + t2*pd[0][2] + tt3*pd[0][3]);
    point.set(0,-(pd[1][1] + t2*pd[1][2] + tt3*pd[1][3]));
}

//Calculate tangent vector at time 0<=t<=1
template<class POINT>
void Bezier<POINT>::TVec( const float t, POINT &point) const
{
    XTRACE();
    float t2 = 2*t;
    float tt3 = 3*t*t;

    for( int d=0; d<point.dimension(); d++)
    {
	point.set(d, pd[d][1] + t2*pd[d][2] + tt3*pd[d][3]);
    }
}
