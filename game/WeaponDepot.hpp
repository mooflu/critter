#pragma once
// Description:
//   Weapon manager.
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

#include <string>
#include <hashMap.hpp>
#include <Singleton.hpp>

#include <HashString.hpp>

class Weapon;

class WeaponDepot
{
friend class Singleton<WeaponDepot>;
public:
    Weapon * getWeapon( std::string weapName);

private:
    ~WeaponDepot();
    WeaponDepot( void);
    WeaponDepot( const WeaponDepot&);
    WeaponDepot &operator=(const WeaponDepot&);

    void addWeapon( Weapon *weap);

    hash_map< std::string, Weapon*, hash<std::string>, std::equal_to<std::string> > _weaponMap;
};

typedef Singleton<WeaponDepot> WeaponDepotS;
