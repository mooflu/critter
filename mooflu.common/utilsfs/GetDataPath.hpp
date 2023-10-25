#pragma once
// Description:
//   Gets the default data path
//
// Copyright (C) 2006 Frank Becker
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
void openURL(const std::string& url);
std::string getDataPath(void);
std::string getPreferencesPath(void);
std::string getDesktopPath(void);
std::string getHomePath(void);
std::string getWritableDataPath(const std::string subDir);
