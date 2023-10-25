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
#include "GLMultiTexture.hpp"

PFNGLACTIVETEXTUREARBPROC GLMultiTexture::_glActiveTextureARB = 0;
PFNGLCLIENTACTIVETEXTUREARBPROC GLMultiTexture::_glClientActiveTextureARB = 0;

PFNGLMULTITEXCOORD1DARBPROC GLMultiTexture::_glMultiTexCoord1dARB = 0;
PFNGLMULTITEXCOORD1DVARBPROC GLMultiTexture::_glMultiTexCoord1dvARB = 0;
PFNGLMULTITEXCOORD1FARBPROC GLMultiTexture::_glMultiTexCoord1fARB = 0;
PFNGLMULTITEXCOORD1FVARBPROC GLMultiTexture::_glMultiTexCoord1fvARB = 0;
PFNGLMULTITEXCOORD1IARBPROC GLMultiTexture::_glMultiTexCoord1iARB = 0;
PFNGLMULTITEXCOORD1IVARBPROC GLMultiTexture::_glMultiTexCoord1ivARB = 0;
PFNGLMULTITEXCOORD1SARBPROC GLMultiTexture::_glMultiTexCoord1sARB = 0;
PFNGLMULTITEXCOORD1SVARBPROC GLMultiTexture::_glMultiTexCoord1svARB = 0;

PFNGLMULTITEXCOORD2DARBPROC GLMultiTexture::_glMultiTexCoord2dARB = 0;
PFNGLMULTITEXCOORD2DVARBPROC GLMultiTexture::_glMultiTexCoord2dvARB = 0;
PFNGLMULTITEXCOORD2FARBPROC GLMultiTexture::_glMultiTexCoord2fARB = 0;
PFNGLMULTITEXCOORD2FVARBPROC GLMultiTexture::_glMultiTexCoord2fvARB = 0;
PFNGLMULTITEXCOORD2IARBPROC GLMultiTexture::_glMultiTexCoord2iARB = 0;
PFNGLMULTITEXCOORD2IVARBPROC GLMultiTexture::_glMultiTexCoord2ivARB = 0;
PFNGLMULTITEXCOORD2SARBPROC GLMultiTexture::_glMultiTexCoord2sARB = 0;
PFNGLMULTITEXCOORD2SVARBPROC GLMultiTexture::_glMultiTexCoord2svARB = 0;

PFNGLMULTITEXCOORD3DARBPROC GLMultiTexture::_glMultiTexCoord3dARB = 0;
PFNGLMULTITEXCOORD3DVARBPROC GLMultiTexture::_glMultiTexCoord3dvARB = 0;
PFNGLMULTITEXCOORD3FARBPROC GLMultiTexture::_glMultiTexCoord3fARB = 0;
PFNGLMULTITEXCOORD3FVARBPROC GLMultiTexture::_glMultiTexCoord3fvARB = 0;
PFNGLMULTITEXCOORD3IARBPROC GLMultiTexture::_glMultiTexCoord3iARB = 0;
PFNGLMULTITEXCOORD3IVARBPROC GLMultiTexture::_glMultiTexCoord3ivARB = 0;
PFNGLMULTITEXCOORD3SARBPROC GLMultiTexture::_glMultiTexCoord3sARB = 0;
PFNGLMULTITEXCOORD3SVARBPROC GLMultiTexture::_glMultiTexCoord3svARB = 0;

PFNGLMULTITEXCOORD4DARBPROC GLMultiTexture::_glMultiTexCoord4dARB = 0;
PFNGLMULTITEXCOORD4DVARBPROC GLMultiTexture::_glMultiTexCoord4dvARB = 0;
PFNGLMULTITEXCOORD4FARBPROC GLMultiTexture::_glMultiTexCoord4fARB = 0;
PFNGLMULTITEXCOORD4FVARBPROC GLMultiTexture::_glMultiTexCoord4fvARB = 0;
PFNGLMULTITEXCOORD4IARBPROC GLMultiTexture::_glMultiTexCoord4iARB = 0;
PFNGLMULTITEXCOORD4IVARBPROC GLMultiTexture::_glMultiTexCoord4ivARB = 0;
PFNGLMULTITEXCOORD4SARBPROC GLMultiTexture::_glMultiTexCoord4sARB = 0;
PFNGLMULTITEXCOORD4SVARBPROC GLMultiTexture::_glMultiTexCoord4svARB = 0;

GLint GLMultiTexture::_numTexUnits = 0;

GLMultiTexture::GLMultiTexture(void) :
    GLExtension(getName()) {
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &_numTexUnits);
    fprintf(stdout, "Found: GL_ARB_multitexture [%d texture units]\n", (int)_numTexUnits);

    _glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)GetExtensionProc("glActiveTextureARB");
    _glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)GetExtensionProc("glClientActiveTextureARB");

    _glMultiTexCoord1dARB = (PFNGLMULTITEXCOORD1DARBPROC)GetExtensionProc("glMultiTexCoord1dARB");
    _glMultiTexCoord1dvARB = (PFNGLMULTITEXCOORD1DVARBPROC)GetExtensionProc("glMultiTexCoord1dvARB");
    _glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC)GetExtensionProc("glMultiTexCoord1fARB");
    _glMultiTexCoord1fvARB = (PFNGLMULTITEXCOORD1FVARBPROC)GetExtensionProc("glMultiTexCoord1fvARB");
    _glMultiTexCoord1iARB = (PFNGLMULTITEXCOORD1IARBPROC)GetExtensionProc("glMultiTexCoord1iARB");
    _glMultiTexCoord1ivARB = (PFNGLMULTITEXCOORD1IVARBPROC)GetExtensionProc("glMultiTexCoord1ivARB");
    _glMultiTexCoord1sARB = (PFNGLMULTITEXCOORD1SARBPROC)GetExtensionProc("glMultiTexCoord1sARB");
    _glMultiTexCoord1svARB = (PFNGLMULTITEXCOORD1SVARBPROC)GetExtensionProc("glMultiTexCoord1svARB");

    _glMultiTexCoord2dARB = (PFNGLMULTITEXCOORD2DARBPROC)GetExtensionProc("glMultiTexCoord2dARB");
    _glMultiTexCoord2dvARB = (PFNGLMULTITEXCOORD2DVARBPROC)GetExtensionProc("glMultiTexCoord2dvARB");
    _glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)GetExtensionProc("glMultiTexCoord2fARB");
    _glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)GetExtensionProc("glMultiTexCoord2fvARB");
    _glMultiTexCoord2iARB = (PFNGLMULTITEXCOORD2IARBPROC)GetExtensionProc("glMultiTexCoord2iARB");
    _glMultiTexCoord2ivARB = (PFNGLMULTITEXCOORD2IVARBPROC)GetExtensionProc("glMultiTexCoord2ivARB");
    _glMultiTexCoord2sARB = (PFNGLMULTITEXCOORD2SARBPROC)GetExtensionProc("glMultiTexCoord2sARB");
    _glMultiTexCoord2svARB = (PFNGLMULTITEXCOORD2SVARBPROC)GetExtensionProc("glMultiTexCoord2svARB");

    _glMultiTexCoord3dARB = (PFNGLMULTITEXCOORD3DARBPROC)GetExtensionProc("glMultiTexCoord3dARB");
    _glMultiTexCoord3dvARB = (PFNGLMULTITEXCOORD3DVARBPROC)GetExtensionProc("glMultiTexCoord3dvARB");
    _glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC)GetExtensionProc("glMultiTexCoord3fARB");
    _glMultiTexCoord3fvARB = (PFNGLMULTITEXCOORD3FVARBPROC)GetExtensionProc("glMultiTexCoord3fvARB");
    _glMultiTexCoord3iARB = (PFNGLMULTITEXCOORD3IARBPROC)GetExtensionProc("glMultiTexCoord3iARB");
    _glMultiTexCoord3ivARB = (PFNGLMULTITEXCOORD3IVARBPROC)GetExtensionProc("glMultiTexCoord3ivARB");
    _glMultiTexCoord3sARB = (PFNGLMULTITEXCOORD3SARBPROC)GetExtensionProc("glMultiTexCoord3sARB");
    _glMultiTexCoord3svARB = (PFNGLMULTITEXCOORD3SVARBPROC)GetExtensionProc("glMultiTexCoord3svARB");

    _glMultiTexCoord4dARB = (PFNGLMULTITEXCOORD4DARBPROC)GetExtensionProc("glMultiTexCoord4dARB");
    _glMultiTexCoord4dvARB = (PFNGLMULTITEXCOORD4DVARBPROC)GetExtensionProc("glMultiTexCoord4dvARB");
    _glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)GetExtensionProc("glMultiTexCoord4fARB");
    _glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)GetExtensionProc("glMultiTexCoord4fvARB");
    _glMultiTexCoord4iARB = (PFNGLMULTITEXCOORD4IARBPROC)GetExtensionProc("glMultiTexCoord4iARB");
    _glMultiTexCoord4ivARB = (PFNGLMULTITEXCOORD4IVARBPROC)GetExtensionProc("glMultiTexCoord4ivARB");
    _glMultiTexCoord4sARB = (PFNGLMULTITEXCOORD4SARBPROC)GetExtensionProc("glMultiTexCoord4sARB");
    _glMultiTexCoord4svARB = (PFNGLMULTITEXCOORD4SVARBPROC)GetExtensionProc("glMultiTexCoord4svARB");
}
