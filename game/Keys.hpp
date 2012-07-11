#pragma once
// Description:
//   Keyboard/Trigger helpers.
//
// Copyright (C) 2007 Frank Becker
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

#include "SDL/SDL.h"

#include "Trigger.hpp"

class Keys
{
public:
    Keys( void);
    ~Keys();
    std::string convertTriggerToString( const Trigger & trigger);
    bool convertStringToTrigger( 
        std::string & triggerStr, Trigger & trigger);

private:
    Keys( const Keys&);
    Keys &operator=(const Keys&);

    std::string _symmap[ SDLK_LAST];
};
