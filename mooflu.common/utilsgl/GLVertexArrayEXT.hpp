#pragma once
// Description:
//   Wrapper for GL_EXT_vertex_array extension.
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
typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
*/
class GLVertexArrayEXT : public GLExtension {
public:
    GLVertexArrayEXT(void);

    virtual const char* getName(void) { return "GL_EXT_vertex_array "; }

    static void glArrayElementEXT(GLint i) { _glArrayElementEXT(i); }

    static void glColorPointerEXT(GLint a, GLenum b, GLsizei c, GLsizei d, const GLvoid* e) {
        _glColorPointerEXT(a, b, c, d, e);
    }

    static void glDrawArraysEXT(GLenum a, GLint b, GLsizei c) { _glDrawArraysEXT(a, b, c); }

    static void glEdgeFlagPointerEXT(GLsizei a, GLsizei b, const GLboolean* c) { _glEdgeFlagPointerEXT(a, b, c); }

    //static void glGetPointervEXT    (GLenum a, GLvoid* *b){_glGetPointervEXT(a,b);}
    static void glIndexPointerEXT(GLenum a, GLsizei b, GLsizei c, const GLvoid* d) { _glIndexPointerEXT(a, b, c, d); }

    static void glNormalPointerEXT(GLenum a, GLsizei b, GLsizei c, const GLvoid* d) { _glNormalPointerEXT(a, b, c, d); }

    static void glTexCoordPointerEXT(GLint a, GLenum b, GLsizei c, GLsizei d, const GLvoid* e) {
        _glTexCoordPointerEXT(a, b, c, d, e);
    }

    static void glVertexPointerEXT(GLint a, GLenum b, GLsizei c, GLsizei d, const GLvoid* e) {
        _glVertexPointerEXT(a, b, c, d, e);
    }

private:
    static PFNGLARRAYELEMENTEXTPROC _glArrayElementEXT;
    static PFNGLCOLORPOINTEREXTPROC _glColorPointerEXT;
    static PFNGLDRAWARRAYSEXTPROC _glDrawArraysEXT;
    static PFNGLEDGEFLAGPOINTEREXTPROC _glEdgeFlagPointerEXT;
    //static PFNGLGETPOINTERVEXTPROC     _glGetPointervEXT;
    static PFNGLINDEXPOINTEREXTPROC _glIndexPointerEXT;
    static PFNGLNORMALPOINTEREXTPROC _glNormalPointerEXT;
    static PFNGLTEXCOORDPOINTEREXTPROC _glTexCoordPointerEXT;
    static PFNGLVERTEXPOINTEREXTPROC _glVertexPointerEXT;
};
