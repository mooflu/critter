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
#include "ResourceManager.hpp"

#include <iomanip>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "Trace.hpp"
#include "FindHash.hpp"
#include "Endian.hpp"
#include "WalkDirectory.hpp"
#include "zStream.hpp"

#ifdef WIN32
const char PATH_SEPERATOR = '\\';
#else
const char PATH_SEPERATOR = '/';
#endif

//Destroy resource manager
ResourceManager::~ResourceManager()
{
    hash_map< const string, DirectoryEntry*, hash<const string> >::const_iterator ci;
    for( ci=_dirEntryMap.begin(); ci!=_dirEntryMap.end(); ci++)
    {
        delete ci->second;
    }
    _dirEntryMap.clear();

    delete _activeInput;
    _activeInput = 0;

    if( _infile.is_open()) _infile.close();
}

void ResourceManager::addResourceDirectory( const string &dirName)
{
    _baseLen = dirName.length()+1;
    addResourceDirectoryRecursive( dirName);
}

void ResourceManager::addResourceDirectoryRecursive( const string &dirName)
{
    WalkDirectory thisDir(dirName);

    DirEntry dirEntry;
    while( thisDir.getNext( dirEntry))
    {
	string fileName = dirEntry.name;
	if( (fileName == ".") || (fileName == "..")) continue;

	string fullName = dirName + PATH_SEPERATOR + fileName;
	if( dirName == ".")
	{
	    fullName = fileName;
	}

	if( dirEntry.type == DirEntry::eDirectory)
	{
	    addResourceDirectoryRecursive( fullName);
	}
	else if( dirEntry.type == DirEntry::eFile)
	{
	    DirectoryEntry *de = new DirectoryEntry;
	    de->resourcePackFilename = fullName;
#ifdef WIN32
	    for( int i=0; i<fullName.length(); i++)
		if( fullName[i] == '\\') fullName[i] = '/';
#endif
	    de->resourceName = fullName.substr( _baseLen, fullName.length()-_baseLen);
	    de->offset = 0;
	    de->origSize = (Uint32)dirEntry.size;
	    de->compSize = (Uint32)dirEntry.size;

	    _dirEntryMap[ de->resourceName] = de;
	}
	else
	{
	    //Handle symlinks?
	}
    }
}

//
bool ResourceManager::addResourcePack( 
  const string &ifilename, const string &basedir)
{
    string filename = ifilename;
    ifstream infile( filename.c_str(), ios::in | ios::binary);
    if( !infile.is_open())
    {
	filename = basedir+ifilename;

	infile.clear();
	infile.open( filename.c_str(), ios::in | ios::binary);
	if( !infile.is_open())
	{
	    LOG_ERROR << "Unable to open resource file: " << filename << endl;
	    return false;
	}
	//LOG_INFO << "Using global resource file." << endl;
    }

    streamoff offset = -HEADER_SIZE;
    infile.seekg( offset, ios::end);

    Uint32 magic;
    Uint32 dirLen;
    infile >> dirLen;
    infile >> magic;

    if( magic != RESOURCE_MAGIC)
    {
	LOG_ERROR << "Incorrect packfile signature! [" 
		  << filename << "]" << endl;
	return false;
    }

    offset = -(streamoff)(dirLen+HEADER_SIZE);
    infile.seekg( offset, ios::end);

    Uint32 dataLen;
    infile >> dataLen;
    dirLen -= 4;

    char *buff = new char[ dirLen];
    infile.read( buff, dirLen);

    //LOG_INFO << "Dirlen = " << dirLen << ", Read = " << (int)infile.gcount() << endl;
    LOG_INFO << "Loading: " << filename << endl;

    string::size_type pos = 0;
    while( pos < dirLen)
    {
	DirectoryEntry *pde = new DirectoryEntry;
        DirectoryEntry &de = *pde;
	de.resourcePackFilename = filename;
	de.resourceName = &buff[pos];
	pos += de.resourceName.length()+1;

	memcpy( &de.offset, &buff[pos], 4);
	pos += 4;

	memcpy( &de.origSize, &buff[pos], 4);
	pos += 4;

	memcpy( &de.compSize, &buff[pos], 4);
	pos += 4;

        if( ! ::isLittleEndian())
        {
	    swap( &de.offset, 4);
	    swap( &de.origSize, 4);
	    swap( &de.compSize, 4);
        }
#if 0        
	LOG_INFO << "  " 
                 << de.resourceName << " ("
		 << de.origSize << "/"
		 << de.compSize << ") "
//                 << " Pos = " << pos
		 << endl;
#endif
	_dirEntryMap[ de.resourceName] = pde;
    }
    
    delete[] buff;

    return true;
}

void ResourceManager::updateInfile( const string &inputFile)
{
    if( inputFile != _openFilename)
    {
	if( _infile.is_open())
	{
	    _infile.close();
	}

	_infile.clear();
        _infile.open( inputFile.c_str(), ios::in | ios::binary);
        _openFilename = inputFile;
    }
}

int ResourceManager::getResourceSize( const string &name)
{
    DirectoryEntry *de = findHash<const string>( name, _dirEntryMap);
    if( !de)
    {
	ifstream tmpfile;
        tmpfile.open( name.c_str(), ios::in | ios::binary);
        if( ! tmpfile.is_open())
	{
	    return -1;
	}
	tmpfile.seekg (0, ios::end);
	int length = tmpfile.tellg();
	return length;
    }
    else
    {
	return de->origSize;
    }

    //shouldn't get here
    return -1;
}

bool ResourceManager::selectResource( const string &name)
{
    if( _activeInput)
    {
	delete _activeInput;
	_activeInput = 0;
    }
    int fileSize = 0;

    //check if resource exists as a plain file
    struct stat statInfo;
    if( ( stat( name.c_str(), &statInfo) == 0) &&
#ifdef VCPP
	( _S_IFREG & statInfo.st_mode))
#else
	( S_ISREG( statInfo.st_mode)))
#endif
    {
	updateInfile( name);
	_infile.clear();
	if( !_infile.is_open()) return false;
	fileSize = statInfo.st_size;
	_infile.seekg( 0);
    }
    else
    {
        //resource not found in filesystem, try resourcepack
	DirectoryEntry *de = findHash<const string>( name, _dirEntryMap);
	if( de)
	{
	    updateInfile( de->resourcePackFilename);
	    _infile.clear();
	    _infile.seekg( de->offset, ios::beg);
	    fileSize = de->origSize;
	}
	else
	{
	    //unable to find resource anywhere
	    return false;
	}
    }

    if( ! _infile.good())
    {
	LOG_ERROR << "INFILE BAD... (gcc3.0 again?)" << endl;
    }

    _activeInput = new ziStream( _infile, fileSize);
    if( !_activeInput)
    {
	return false;
    }

    if( !_activeInput->isOK())
    {
	delete _activeInput;
	_activeInput = 0;
	return false;
    }

    return true;
}

ziStream &ResourceManager::getInputStream( void)
{
    return( *_activeInput);
}

void ResourceManager::getResourceList( list<string> &rNameList)
{
    hash_map< const string, DirectoryEntry*, hash<const string> >::const_iterator ci;
    for( ci=_dirEntryMap.begin(); ci!=_dirEntryMap.end(); ci++)
    {
        rNameList.insert( rNameList.end(), ci->second->resourceName);
    }
}

void ResourceManager::dump( void)
{
    int totalOrig = 0;
    int totalComp = 0;

    hash_map< const string, DirectoryEntry*, hash<const string> >::const_iterator ci;
    for( ci=_dirEntryMap.begin(); ci!=_dirEntryMap.end(); ci++)
    {
        DirectoryEntry &di = *(ci->second);
        string name = di.resourcePackFilename +":"+ di.resourceName;
        LOG_INFO.width(50);
        LOG_VOID.fill('.');
        LOG_VOID.setf( ios::left);
        LOG_VOID << name.c_str();

        LOG_VOID.width(12);
        LOG_VOID.setf( ios::right);
        LOG_VOID << di.origSize; 

        LOG_VOID << "  (" << di.compSize << ")" << endl;

        totalOrig += di.origSize;
        totalComp += di.compSize;
    }
    LOG_INFO << totalOrig << " (" << totalComp << ") bytes." << endl;
}
