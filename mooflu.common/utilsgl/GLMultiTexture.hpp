#pragma once
// Description:
//   Wrapper for GL_ARB_multitexture extension.
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

#include "GLExtension.hpp"

/*
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum target, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum target, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum target, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum target, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum target, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
*/
class GLMultiTexture : public GLExtension {
public:
    GLMultiTexture(void);

    virtual const char* getName(void) { return "GL_ARB_multitexture "; }

    static void glActiveTextureARB(GLenum tex) { _glActiveTextureARB(tex); }

    static void glClientActiveTextureARB(GLenum x) { _glClientActiveTextureARB(x); }

    static void glMultiTexCoord1dARB(GLenum a, GLdouble b) { _glMultiTexCoord1dARB(a, b); }

    static void glMultiTexCoord1dvARB(GLenum a, const GLdouble* b) { _glMultiTexCoord1dvARB(a, b); }

    static void glMultiTexCoord1fARB(GLenum a, GLfloat b) { _glMultiTexCoord1fARB(a, b); }

    static void glMultiTexCoord1fvARB(GLenum a, const GLfloat* b) { _glMultiTexCoord1fvARB(a, b); }

    static void glMultiTexCoord1iARB(GLenum a, GLint b) { _glMultiTexCoord1iARB(a, b); }

    static void glMultiTexCoord1ivARB(GLenum a, const GLint* b) { _glMultiTexCoord1ivARB(a, b); }

    static void glMultiTexCoord1sARB(GLenum a, GLshort b) { _glMultiTexCoord1sARB(a, b); }

    static void glMultiTexCoord1svARB(GLenum a, const GLshort* b) { _glMultiTexCoord1svARB(a, b); }

    static void glMultiTexCoord2dARB(GLenum a, GLdouble b, GLdouble c) { _glMultiTexCoord2dARB(a, b, c); }

    static void glMultiTexCoord2dvARB(GLenum a, const GLdouble* b) { _glMultiTexCoord2dvARB(a, b); }

    static void glMultiTexCoord2fARB(GLenum a, GLfloat b, GLfloat c) { _glMultiTexCoord2fARB(a, b, c); }

    static void glMultiTexCoord2fvARB(GLenum a, const GLfloat* b) { _glMultiTexCoord2fvARB(a, b); }

    static void glMultiTexCoord2iARB(GLenum a, GLint b, GLint c) { _glMultiTexCoord2iARB(a, b, c); }

    static void glMultiTexCoord2ivARB(GLenum a, const GLint* b) { _glMultiTexCoord2ivARB(a, b); }

    static void glMultiTexCoord2sARB(GLenum a, GLshort b, GLshort c) { _glMultiTexCoord2sARB(a, b, c); }

    static void glMultiTexCoord2svARB(GLenum a, const GLshort* b) { _glMultiTexCoord2svARB(a, b); }

    static void glMultiTexCoord3dARB(GLenum a, GLdouble b, GLdouble c, GLdouble d) {
        _glMultiTexCoord3dARB(a, b, c, d);
    }

    static void glMultiTexCoord3dvARB(GLenum a, const GLdouble* b) { _glMultiTexCoord3dvARB(a, b); }

    static void glMultiTexCoord3fARB(GLenum a, GLfloat b, GLfloat c, GLfloat d) { _glMultiTexCoord3fARB(a, b, c, d); }

    static void glMultiTexCoord3fvARB(GLenum a, const GLfloat* b) { _glMultiTexCoord3fvARB(a, b); }

    static void glMultiTexCoord3iARB(GLenum a, GLint b, GLint c, GLint d) { _glMultiTexCoord3iARB(a, b, c, d); }

    static void glMultiTexCoord3ivARB(GLenum a, const GLint* b) { _glMultiTexCoord3ivARB(a, b); }

    static void glMultiTexCoord3sARB(GLenum a, GLshort b, GLshort c, GLshort d) { _glMultiTexCoord3sARB(a, b, c, d); }

    static void glMultiTexCoord3svARB(GLenum a, const GLshort* b) { _glMultiTexCoord3svARB(a, b); }

    static void glMultiTexCoord4dARB(GLenum a, GLdouble b, GLdouble c, GLdouble d, GLdouble e) {
        _glMultiTexCoord4dARB(a, b, c, d, e);
    }

    static void glMultiTexCoord4dvARB(GLenum a, const GLdouble* b) { _glMultiTexCoord4dvARB(a, b); }

    static void glMultiTexCoord4fARB(GLenum a, GLfloat b, GLfloat c, GLfloat d, GLfloat e) {
        _glMultiTexCoord4fARB(a, b, c, d, e);
    }

    static void glMultiTexCoord4fvARB(GLenum a, const GLfloat* b) { _glMultiTexCoord4fvARB(a, b); }

    static void glMultiTexCoord4iARB(GLenum a, GLint b, GLint c, GLint d, GLint e) {
        _glMultiTexCoord4iARB(a, b, c, d, e);
    }

    static void glMultiTexCoord4ivARB(GLenum a, const GLint* b) { _glMultiTexCoord4ivARB(a, b); }

    static void glMultiTexCoord4sARB(GLenum a, GLshort b, GLshort c, GLshort d, GLshort e) {
        _glMultiTexCoord4sARB(a, b, c, d, e);
    }

    static void glMultiTexCoord4svARB(GLenum a, const GLshort* b) { _glMultiTexCoord4svARB(a, b); }

private:
    static PFNGLACTIVETEXTUREARBPROC _glActiveTextureARB;
    static PFNGLCLIENTACTIVETEXTUREARBPROC _glClientActiveTextureARB;
    static PFNGLMULTITEXCOORD1DARBPROC _glMultiTexCoord1dARB;
    static PFNGLMULTITEXCOORD1DVARBPROC _glMultiTexCoord1dvARB;
    static PFNGLMULTITEXCOORD1FARBPROC _glMultiTexCoord1fARB;
    static PFNGLMULTITEXCOORD1FVARBPROC _glMultiTexCoord1fvARB;
    static PFNGLMULTITEXCOORD1IARBPROC _glMultiTexCoord1iARB;
    static PFNGLMULTITEXCOORD1IVARBPROC _glMultiTexCoord1ivARB;
    static PFNGLMULTITEXCOORD1SARBPROC _glMultiTexCoord1sARB;
    static PFNGLMULTITEXCOORD1SVARBPROC _glMultiTexCoord1svARB;
    static PFNGLMULTITEXCOORD2DARBPROC _glMultiTexCoord2dARB;
    static PFNGLMULTITEXCOORD2DVARBPROC _glMultiTexCoord2dvARB;
    static PFNGLMULTITEXCOORD2FARBPROC _glMultiTexCoord2fARB;
    static PFNGLMULTITEXCOORD2FVARBPROC _glMultiTexCoord2fvARB;
    static PFNGLMULTITEXCOORD2IARBPROC _glMultiTexCoord2iARB;
    static PFNGLMULTITEXCOORD2IVARBPROC _glMultiTexCoord2ivARB;
    static PFNGLMULTITEXCOORD2SARBPROC _glMultiTexCoord2sARB;
    static PFNGLMULTITEXCOORD2SVARBPROC _glMultiTexCoord2svARB;
    static PFNGLMULTITEXCOORD3DARBPROC _glMultiTexCoord3dARB;
    static PFNGLMULTITEXCOORD3DVARBPROC _glMultiTexCoord3dvARB;
    static PFNGLMULTITEXCOORD3FARBPROC _glMultiTexCoord3fARB;
    static PFNGLMULTITEXCOORD3FVARBPROC _glMultiTexCoord3fvARB;
    static PFNGLMULTITEXCOORD3IARBPROC _glMultiTexCoord3iARB;
    static PFNGLMULTITEXCOORD3IVARBPROC _glMultiTexCoord3ivARB;
    static PFNGLMULTITEXCOORD3SARBPROC _glMultiTexCoord3sARB;
    static PFNGLMULTITEXCOORD3SVARBPROC _glMultiTexCoord3svARB;
    static PFNGLMULTITEXCOORD4DARBPROC _glMultiTexCoord4dARB;
    static PFNGLMULTITEXCOORD4DVARBPROC _glMultiTexCoord4dvARB;
    static PFNGLMULTITEXCOORD4FARBPROC _glMultiTexCoord4fARB;
    static PFNGLMULTITEXCOORD4FVARBPROC _glMultiTexCoord4fvARB;
    static PFNGLMULTITEXCOORD4IARBPROC _glMultiTexCoord4iARB;
    static PFNGLMULTITEXCOORD4IVARBPROC _glMultiTexCoord4ivARB;
    static PFNGLMULTITEXCOORD4SARBPROC _glMultiTexCoord4sARB;
    static PFNGLMULTITEXCOORD4SVARBPROC _glMultiTexCoord4svARB;

    static GLint _numTexUnits;
};
