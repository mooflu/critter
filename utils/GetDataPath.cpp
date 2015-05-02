// Description:
//   Gets the default data path
//
// Copyright (C) 2006 Frank Becker
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
#include "GetDataPath.hpp"

#include "Trace.hpp"

#include <string>

#ifdef __APPLE__
#ifdef IPHONE   
#import <Foundation/Foundation.h>
#define kPreferencesFolderType 0
#define kCurrentUserFolderType 1
#define kDesktopFolderType 2
#define kApplicationSupportFolderType 3
#else
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#endif
#endif

#ifndef VCPP
#include <unistd.h>
#include <stdlib.h>
#else
#include <shlobj.h>
#include <direct.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __APPLE__
static inline std::string getPath( OSType folderType, bool create = true)
{
    std::string thePath;
#ifndef IPHONE   
    FSRef pathRef;
    OSErr err = FSFindFolder(kUserDomain, folderType, create ? kCreateFolder : kDontCreateFolder, &pathRef);
    if( err == noErr)
    {
        char path[PATH_MAX];
        FSRefMakePath(&pathRef, (UInt8*)path, sizeof(path));
        thePath = path;
    }
#else
    NSArray *paths = NSSearchPathForDirectoriesInDomains( 
        NSApplicationSupportDirectory,
        NSUserDomainMask,
        YES                                                
    );
    
    if ([paths count] > 0)
    {
        NSMutableString *appSupportPath = [NSMutableString stringWithString:[paths objectAtIndex:0]];
        thePath = [appSupportPath cStringUsingEncoding:NSUTF8StringEncoding];
        [[NSFileManager defaultManager] createDirectoryAtPath:appSupportPath 
                                  withIntermediateDirectories:YES 
                                                   attributes:nil 
                                                        error:NULL];
    }
    LOG_INFO << "getPath:" << thePath << "\n";
#endif
    return thePath;
}
#elif defined(WIN32)
static inline std::string getPath( int folderType )
{
    std::string thePath;

    CHAR szPath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathA(NULL, 
                             folderType | CSIDL_FLAG_CREATE, 
                             NULL, 
                             0, 
                             szPath))) 
    {
	thePath = szPath;
    }
    else
    {
	thePath = "./";
    }
    return thePath;
}
#endif

std::string getDataPath( void)
{
    std::string appPath = "";
#ifdef __APPLE__
    CFURLRef appURLRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appURLRef, 
	    kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath, 
	    CFStringGetSystemEncoding());
    if( pathPtr)
	appPath = pathPtr;

    CFRelease(appURLRef);
    CFRelease(macPath);

#ifdef IPHONE
    appPath = appPath + "/";
#else
    appPath = appPath + "/Contents/Resources/";
#endif
#elif defined(WIN32)
    appPath = std::string("./");
#else
    appPath = std::string(DATA_DIR);
#endif
    return appPath;
}

std::string getPreferencesPath( void)
{
    std::string prefPath = "";
#ifdef __APPLE__
    prefPath = getPath( kPreferencesFolderType);
    if( prefPath.empty())
    {
        prefPath = getPath( kCurrentUserFolderType);
    }
#elif defined(WIN32)
    prefPath = getPath( CSIDL_APPDATA);
#else
    prefPath = getenv("HOME");
#endif    
    return prefPath;
}

std::string getDesktopPath( void)
{
    std::string dtPath = "";
#ifdef __APPLE__
    dtPath = getPath( kDesktopFolderType);
    if( dtPath.empty())
    {
        dtPath = getPath( kCurrentUserFolderType);
    }
#elif defined(WIN32)
    dtPath = getPath( CSIDL_DESKTOPDIRECTORY);
#else
    dtPath = getenv("HOME");
    dtPath += "/Desktop";
#endif    
    return dtPath;
}

std::string getHomePath( void)
{
    std::string homePath = "";
#ifdef __APPLE__
    homePath = getPath( kCurrentUserFolderType);
#elif defined(WIN32)
    homePath = getPath( CSIDL_MYDOCUMENTS);
#else
    homePath = getenv("HOME");
#endif    
    return homePath;
}

#if defined(__APPLE__) || defined(WIN32)
std::string getWritableDataPath( const std::string subDir)
#else
std::string getWritableDataPath( const std::string /*subDir*/)
#endif
{
    std::string writableDataPath = "";
#ifdef __APPLE__
    writableDataPath = getPath( kApplicationSupportFolderType);
    if( writableDataPath.empty())
    {
        writableDataPath = getPath( kCurrentUserFolderType);
    }
    writableDataPath += "/";
    
    if( subDir.length())
    {
        writableDataPath += subDir + "/";
        std::string::size_type start = writableDataPath.find_last_of( '/');
        if( start > 0)
        {
            struct stat statInfo;
            std::string path = writableDataPath.substr( 0, start);
            if( (stat( path.c_str(), &statInfo) == -1) )
            {
                mkdir( path.c_str(), 0777);
            }
        }
    }    
    
#elif defined(WIN32)
    writableDataPath = getPath( CSIDL_APPDATA) + "/" + subDir;
    _mkdir( writableDataPath.c_str());
#else
    writableDataPath = getenv("HOME");
#endif    
    return writableDataPath;
}

