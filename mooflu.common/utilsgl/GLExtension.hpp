#pragma once
// Description:
//   GL extension helpers.
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

extern "C" { typedef void (*__GL_EXT_FuncPtr)(); }

#define GLEXTCAST (__GL_EXT_FuncPtr)

#include "SDL.h"

#include <stdio.h>

#include <GL/glew.h>
#include "Trace.hpp"

// TODO: replace GLExtension with checking via glew:
// E.g. if (GLEW_ARB_vertex_program)

class GLExtension {
public:
    GLExtension(const char* extensionName) {
        //check if extension is supported
        _isSupported = false;

        GLint n = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        for (GLint i = 0; i < n; i++) {
            const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
            if (strstr(extension, extensionName)) {
                LOG_INFO << "Supported: [" << extensionName << "]\n";
                _isSupported = true;
                break;
            }
        }
    }

    virtual ~GLExtension() {}

    virtual const char* getName(void) = 0;

    static void close(void) {
    }

    bool isSupported(void) { return _isSupported; }

protected:
    __GL_EXT_FuncPtr GetExtensionProc(const char* fName) {
        __GL_EXT_FuncPtr fn = (__GL_EXT_FuncPtr)SDL_GL_GetProcAddress(fName);
        if (!fn) {
            LOG_ERROR << getName() << ": Unable to get function pointer for " << fName << "\n";
        }
        return fn;
    }

private:
    GLExtension(void);
    GLExtension(const GLExtension&);
    GLExtension& operator=(const GLExtension&);

    bool _isSupported;
};
