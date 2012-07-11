#pragma once

/*
 * Ripped from: Mesa 3-D graphics library
 * Version:  3.5
 * Copyright (C) 1995-2001  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if 0
static inline void
frustum(GLdouble left, GLdouble right,
	GLdouble bottom, GLdouble top,
	GLdouble nearval, GLdouble farval)
{
    GLdouble x, y, a, b, c, d;
    GLdouble m[16];

    x = (2.0 * nearval) / (right - left);
    y = (2.0 * nearval) / (top - bottom);
    a = (right + left) / (right - left);
    b = (top + bottom) / (top - bottom);
    c = -(farval + nearval) / ( farval - nearval);
    d = -(2.0 * farval * nearval) / (farval - nearval);

#define M(row,col)  m[col*4+row]
    M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
    M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
    M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
    M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

    glMultMatrixd(m);
}

static inline void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble xmin, xmax, ymin, ymax;

    ymax = zNear * tan(fovy * M_PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    /* don't call glFrustum() because of error semantics (covglu) */
    frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

static inline void gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
    glOrtho(left, right, bottom, top, -1.0, 1.0);
}
#endif
