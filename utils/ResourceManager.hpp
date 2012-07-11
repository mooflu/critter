#pragma once
// Description:
//   Simple pak file type resouce manager.
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

#include <list>
#include <string>

#include "hashMap.hpp"
#include "HashString.hpp"
#include "Singleton.hpp"
#include "zStream.hpp"

struct DirectoryEntry
{
    std::string resourceName;
    std::string resourcePackFilename;
    Uint32 offset;
    Uint32 origSize;
    Uint32 compSize;
};

const Uint32 RESOURCE_MAGIC = 0xfbfb3755;

class ResourceManager
{
friend class Singleton<ResourceManager>;
public:
    bool addResourcePack( const std::string &filename, const std::string &basedir = "");
    void addResourceDirectory( const std::string &dirName);

    bool selectResource( const std::string &name);
    int getResourceSize( const std::string &name);
    int readResource( char *buffer, int size)
    {
        _activeInput->read( buffer, size);
        return size;
    }
    ziStream &getInputStream( void);

    void getResourceList( std::list<std::string> &rNameList);
    void dump( void);

private:
    ResourceManager( const ResourceManager&);
    ResourceManager &operator=(const ResourceManager&);

    ResourceManager( void):
        HEADER_SIZE(8),
        _openFilename(""),
        _activeInput(0)
    {
    }
    ~ResourceManager();
    void updateInfile( const std::string &inputFilename);
    void addResourceDirectoryRecursive( const std::string &dirName);

    const std::streamoff HEADER_SIZE; //8;

    std::string _openFilename;
    std::ifstream _infile;
    ziStream *_activeInput;
	hash_map< const std::string, DirectoryEntry*, hash<const std::string>, std::equal_to<const std::string> > _dirEntryMap;
    std::string::size_type _baseLen;
};

typedef Singleton<ResourceManager> ResourceManagerS;
