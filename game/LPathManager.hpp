#pragma once
// Description:
//   Path manager.
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

#include <ResourceCache.hpp>
#include <Singleton.hpp>
#include <LevelPack.hpp>

class LPathManager: public ResourceCache<LPath>
{
friend class Singleton<LPathManager>;
public:
    LPath *getPath( const string &pathName)
    { 
	return getResource( pathName);
    }

protected:
    virtual LPath *load( const string &path);

private:
    LPathManager( void);
    virtual ~LPathManager();
    LPathManager( const LPathManager&);
    LPathManager &operator=(const LPathManager&);
};

typedef Singleton<LPathManager> LPathManagerS;
;
