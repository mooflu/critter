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

class GLExtension {
public:
    GLExtension(const char* extensionName) {
        //get all extensions, pad with space
        if (!_paddedExtensions) {
            const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
            _paddedExtensions = new char[strlen(extensions) + 2];
            strcat(strcpy(_paddedExtensions, extensions), " ");
            //	    LOG_INFO << "[" << _paddedExtensions << "]\n";
        }

        _extensionName = new char[strlen(extensionName) + 1];
        strcpy(_extensionName, extensionName);

        //check if extension is supported
        _isSupported = false;
        if (strstr(_paddedExtensions, _extensionName)) {
            LOG_INFO << "Supported: [" << _extensionName << "]\n";
            _isSupported = true;
        }
    }

    virtual ~GLExtension() { delete[] _extensionName; }

    virtual const char* getName(void) = 0;

    static void close(void) {
        delete[] _paddedExtensions;
        _paddedExtensions = 0;
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
    char* _extensionName;
    static char* _paddedExtensions;
};
