#pragma once
// Description:
//   Base for callbacks.
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
#include "Input.hpp"

class Callback
{
public:
    Callback( const std::string &actionName, const std::string &defaultTrigger);
    virtual ~Callback();

    virtual void performAction( Trigger &trigger, bool isDown) = 0;
    std::string &getActionName( void){ return _actionName;};
    std::string &getDefaultTriggerName( void){ return _defaultTrigger;};

protected:
    std::string _actionName;
    std::string _defaultTrigger;

private:
    Callback( const Callback&);
    Callback &operator=(const Callback&);
};
