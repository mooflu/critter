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
#include "WeaponDepot.hpp"

#include <Trace.hpp>
#include <Weapon.hpp>
#include <FindHash.hpp>

using namespace std;

WeaponDepot::WeaponDepot( void)
{
    XTRACE();

    addWeapon( new GuardDog());
    addWeapon( new WingPhaser());
    addWeapon( new TripplePhaser());
    addWeapon( new IceSpray());
    addWeapon( new FlankBurster());
    addWeapon( new Stinger());
}

WeaponDepot::~WeaponDepot()
{
    XTRACE();

    hash_map< string, Weapon*, hash<string> >::const_iterator ci;
    for( ci=_weaponMap.begin(); ci!=_weaponMap.end(); ci++)
    {
        delete ci->second;
    }

    _weaponMap.clear();
}

Weapon * WeaponDepot::getWeapon( string weapName)
{
//    XTRACE();
    Weapon * weap = findHash( weapName, _weaponMap);
    if( !weap)
    {
        LOG_ERROR << "Weapon Depot never heard of a " << weapName 
                  << " before!" << endl;

        //trying first in list
        weap = _weaponMap.begin()->second;
    }
    return weap;
}

void WeaponDepot::addWeapon( Weapon *weap)
{
    XTRACE();
    if( weap)
    {
        LOG_INFO << "New Weapon type: [" << weap->name() << "] [" << weap->description() << "]" << endl;
        _weaponMap[ weap->name()] = weap;
    }
}
