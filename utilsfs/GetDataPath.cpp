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
#include <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#define kPreferencesFolderType 0
#define kCurrentUserFolderType 1
#define kDesktopFolderType 2
#define kApplicationSupportFolderType 3
#else
#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#endif
#endif

#ifndef _MSC_VER
#include <unistd.h>
#include <stdlib.h>
#else
#include <Windows.h>
#include <Shellapi.h>
#include <shlobj.h>
#include <direct.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __APPLE__
void openURL(const std::string& url) {
    NSURL* nsURL = [NSURL URLWithString:[NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding]];
#ifdef IPHONE
    [[UIApplication sharedApplication] openURL:nsURL];
#else
    [[NSWorkspace sharedWorkspace] openURL:nsURL];
#endif
}

static inline std::string getPath(OSType folderType, bool create = true) {
    std::string thePath;
#ifndef IPHONE
    FSRef pathRef;
    OSErr err = FSFindFolder(kUserDomain, folderType, create ? kCreateFolder : kDontCreateFolder, &pathRef);
    if (err == noErr) {
        char path[PATH_MAX];
        FSRefMakePath(&pathRef, (UInt8*)path, sizeof(path));
        thePath = path;
    }
#else
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);

    if ([paths count] > 0) {
        NSMutableString* appSupportPath = [NSMutableString stringWithString:[paths objectAtIndex:0]];
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
void openURL(const std::string& url) {
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

static inline std::string getPath(int folderType) {
    std::string thePath;

    CHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, folderType | CSIDL_FLAG_CREATE, NULL, 0, szPath))) {
        thePath = szPath;
    } else {
        thePath = "./";
    }
    return thePath;
}
#else
void openURL(const std::string& url) {}
#endif

std::string getDataPath(void) {
#if defined(EMSCRIPTEN)
    return std::string("/");
#endif
    std::string appPath = "";
#ifdef __APPLE__
    CFURLRef appURLRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appURLRef, kCFURLPOSIXPathStyle);
    const char* pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
    if (pathPtr) {
        appPath = pathPtr;
    }

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
#if !defined(DATA_DIR)
#define DATA_DIR "./"  //TODO: make this a config item?
#endif
    appPath = std::string(DATA_DIR);
#endif
    return appPath;
}

std::string getPreferencesPath(void) {
    std::string prefPath = "";
#ifdef __APPLE__
    prefPath = getPath(kPreferencesFolderType);
    if (prefPath.empty()) {
        prefPath = getPath(kCurrentUserFolderType);
    }
#elif defined(WIN32)
    prefPath = getPath(CSIDL_APPDATA);
#else
    prefPath = getenv("HOME");
#endif
    return prefPath;
}

std::string getDesktopPath(void) {
    std::string dtPath = "";
#ifdef __APPLE__
    dtPath = getPath(kDesktopFolderType);
    if (dtPath.empty()) {
        dtPath = getPath(kCurrentUserFolderType);
    }
#elif defined(WIN32)
    dtPath = getPath(CSIDL_DESKTOPDIRECTORY);
#else
    dtPath = getenv("HOME");
    dtPath += "/Desktop";
#endif
    return dtPath;
}

std::string getHomePath(void) {
    std::string homePath = "";
#ifdef __APPLE__
    homePath = getPath(kCurrentUserFolderType);
#elif defined(WIN32)
    homePath = getPath(CSIDL_MYDOCUMENTS);
#else
    homePath = getenv("HOME");
#endif
    return homePath;
}

std::string getWritableDataPath(const std::string subDir) {
#if defined(EMSCRIPTEN)
    return std::string(subDir);
#endif

    std::string writableDataPath = "";
#ifdef __APPLE__
    writableDataPath = getPath(kApplicationSupportFolderType);
    if (writableDataPath.empty()) {
        writableDataPath = getPath(kCurrentUserFolderType);
    }
    writableDataPath += "/" + subDir;
#elif defined(WIN32)
    writableDataPath = getPath(CSIDL_APPDATA) + "/" + subDir;
#else
    writableDataPath = std::string(getenv("HOME")) + "/" + subDir;
#endif
    if (!subDir.empty()) {
        writableDataPath += "/";
    }

    std::string::size_type start = writableDataPath.find_last_of('/');
    if (start > 0) {
        struct stat statInfo;
        std::string path = writableDataPath.substr(0, start);
        if ((stat(path.c_str(), &statInfo) == -1)) {
#if defined(WIN32)
            _mkdir(path.c_str());
#else
            mkdir(path.c_str(), 0777);
#endif
        }
    }

    return writableDataPath;
}
