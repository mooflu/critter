#pragma once
// Description:
//   Notification interface for loading and saving configuration.
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
#include <iostream>

class ConfigHandler {
public:
    //when reading the config file, registered ConfigHandlers
    //will be notified for each line read.
    virtual void handleLine(const std::string line) = 0;

    //when writing the config file, registered ConfigHandlers
    //will be notified with the output stream
    virtual void save(std::ostream& o) = 0;

    virtual ~ConfigHandler() {}
};
