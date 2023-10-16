#pragma once
// Description:
//   Tell my thy OS.
//
// Copyright (C) 2010 Frank Becker
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

#if defined(WIN32)
#include <Windows.h>
#include <sstream>

static inline std::string OSNAME() {
    BOOL bIsWow64 = FALSE;
    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

    LPFN_ISWOW64PROCESS
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process");

    if (NULL != fnIsWow64Process) {
        if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
            // handle error
        }
    }
    std::string bitness = "32";
    if (bIsWow64) {
        bitness = "64";
    }

    OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);
    std::stringstream version;
    version << "Windows" << bitness << "-" << 
	osvi.dwMajorVersion << "." <<
	osvi.dwMinorVersion << "." <<
	osvi.dwBuildNumber << std::ends;

    return version.str();
}
#elif defined (__APPLE__)
  #if defined (__POWERPC__)
    #define OSNAME() "OSX-ppc"
  #elif defined (__ppc64__)
    #define OSNAME() "OSX-ppc64"
  #elif defined (__i386__)
    #define OSNAME() "OSX-x86"
  #else
    #define OSNAME() "OSX"
  #endif
#elif defined (__linux__)
  #if defined (__i386__)
    #define OSNAME() "Linux-x86"
  #elif defined (__x86_64__)
    #define OSNAME() "Linux-x86_64"
  #elif defined (__POWERPC__)
    #define OSNAME() "Linux-ppc"
  #elif defined (__alpha__)
    #define OSNAME() "Linux-alpha"
  #else
    #define OSNAME() "Linux"
  #endif
#elif defined (__NetBSD__)
  #define OSNAME() "NetBSD"
#elif defined (__OpenBSD__)
  #define OSNAME() "OpenBSD"
#elif defined (__FreeBSD__)
  #define OSNAME() "FreeBSD"
#elif defined (__sun__)
  #if defined (__sparc__)
    #define OSNAME() "Solaris-sparc"
  #elif defined (__i386__)
    #define OSNAME() "Solaris-x86"
  #else
    #define OSNAME() "Solaris"
  #endif
#elif defined (__sgi__)
  #define OSNAME() "SGI"
#elif defined (__hpux__)
  #define OSNAME() "HPUX"
#else
  #define OSNAME() "Other"
#endif
