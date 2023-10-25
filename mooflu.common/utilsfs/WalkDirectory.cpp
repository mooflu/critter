// Description:
//   Directory entry walker
//
// Copyright (C) 2004 Frank Becker
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
#include "WalkDirectory.hpp"

#ifdef _MSC_VER
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "Trace.hpp"

WalkDirectory::WalkDirectory(const std::string& dir) {
#ifdef _MSC_VER
    _nextAvail = false;
    std::string wildCardDir = dir + "\\*.*";
    _dirHandle = FindFirstFileA(wildCardDir.c_str(), &_dirEntry);
    if (_dirHandle != INVALID_HANDLE_VALUE) {
        _nextAvail = true;
    }
#else
    _directory = dir;
    _dirHandle = opendir(dir.c_str());
#endif
}

WalkDirectory::~WalkDirectory() {
#ifdef _MSC_VER
    if (_dirHandle != INVALID_HANDLE_VALUE) {
        FindClose(_dirHandle);
    }
#else
    if (_dirHandle) {
        closedir(_dirHandle);
    }
#endif
}

bool WalkDirectory::getNext(DirEntry& dirEntry) {
#ifdef _MSC_VER
    if (!_nextAvail) {
        return false;
    }

    dirEntry.name = _dirEntry.cFileName;
    if (_dirEntry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        dirEntry.type = DirEntry::eDirectory;
    } else if (_dirEntry.dwFileAttributes & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE)) {
        dirEntry.type = DirEntry::eFile;
    } else {
        dirEntry.type = DirEntry::eOther;
    }

    dirEntry.size = _dirEntry.nFileSizeLow;

    _nextAvail = FindNextFileA(_dirHandle, &_dirEntry);
#else
    if (!_dirHandle) {
        return false;
    }

    struct dirent* dirEnt = readdir(_dirHandle);
    if (!dirEnt) {
        return false;
    }

    dirEntry.name = dirEnt->d_name;
    std::string fullPath = _directory + "/" + dirEntry.name;
    struct stat statInfo;
    if (stat(fullPath.c_str(), &statInfo) == -1) {
        LOG_ERROR << "Failed stat on " << fullPath << "\n";
        return true;
    }

    if (S_ISDIR(statInfo.st_mode)) {
        dirEntry.type = DirEntry::eDirectory;
    } else if (S_ISREG(statInfo.st_mode)) {
        dirEntry.type = DirEntry::eFile;
    } else {
        dirEntry.type = DirEntry::eOther;
    }

    dirEntry.size = statInfo.st_size;
#endif

    return true;
}
