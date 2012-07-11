#pragma once
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

#include <string>

#ifdef VCPP
#include <windows.h>
#else
#include <dirent.h>
#endif

struct DirEntry
{
    enum EntryEnum
    {
	eDirectory,
	eFile,
	eOther
    };

    std::string name;
    EntryEnum type;
    size_t size;
};

class WalkDirectory
{
public:
    WalkDirectory(const std::string &dir);
    ~WalkDirectory();

    bool getNext( DirEntry &dirEntry);

private:
#ifdef VCPP
    BOOL _nextAvail;
    HANDLE _dirHandle;
    WIN32_FIND_DATAA _dirEntry;
#else
    std::string _directory;
    DIR *_dirHandle;
#endif

};
