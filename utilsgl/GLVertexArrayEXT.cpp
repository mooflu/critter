// Description:
//   Wrapper for GL_ARB_texture_cube_map extension.
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
#include "GLVertexArrayEXT.hpp"

PFNGLARRAYELEMENTEXTPROC GLVertexArrayEXT::_glArrayElementEXT = 0;
PFNGLCOLORPOINTEREXTPROC GLVertexArrayEXT::_glColorPointerEXT = 0;
PFNGLDRAWARRAYSEXTPROC GLVertexArrayEXT::_glDrawArraysEXT = 0;
PFNGLEDGEFLAGPOINTEREXTPROC GLVertexArrayEXT::_glEdgeFlagPointerEXT = 0;
//PFNGLGETPOINTERVEXTPROC     GLVertexArrayEXT::_glGetPointervEXT = 0;
PFNGLINDEXPOINTEREXTPROC GLVertexArrayEXT::_glIndexPointerEXT = 0;
PFNGLNORMALPOINTEREXTPROC GLVertexArrayEXT::_glNormalPointerEXT = 0;
PFNGLTEXCOORDPOINTEREXTPROC GLVertexArrayEXT::_glTexCoordPointerEXT = 0;
PFNGLVERTEXPOINTEREXTPROC GLVertexArrayEXT::_glVertexPointerEXT = 0;

GLVertexArrayEXT::GLVertexArrayEXT(void) :
    GLExtension(getName()) {
    _glArrayElementEXT = (PFNGLARRAYELEMENTEXTPROC)GetExtensionProc("glArrayElementEXT");
    _glColorPointerEXT = (PFNGLCOLORPOINTEREXTPROC)GetExtensionProc("glColorPointerEXT");
    _glDrawArraysEXT = (PFNGLDRAWARRAYSEXTPROC)GetExtensionProc("glDrawArraysEXT");
    _glEdgeFlagPointerEXT = (PFNGLEDGEFLAGPOINTEREXTPROC)GetExtensionProc("glEdgeFlagPointerEXT");
    //_glGetPointervEXT     = (PFNGLGETPOINTERVEXTPROC)GetExtensionProc("glGetPointervEXT");
    _glIndexPointerEXT = (PFNGLINDEXPOINTEREXTPROC)GetExtensionProc("glIndexPointerEXT");
    _glNormalPointerEXT = (PFNGLNORMALPOINTEREXTPROC)GetExtensionProc("glNormalPointerEXT");
    _glTexCoordPointerEXT = (PFNGLTEXCOORDPOINTEREXTPROC)GetExtensionProc("glTexCoordPointerEXT");
    _glVertexPointerEXT = (PFNGLVERTEXPOINTEREXTPROC)GetExtensionProc("glVertexPointerEXT");
}
