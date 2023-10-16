#pragma once
// Description:
//   SGI/GCC hash_map STL extension.
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
#ifndef HASH_NAMESPACE

//needs C++0x

#define HASH_NAMESPACE std
#include <unordered_map>
#define hash_map unordered_map
using namespace HASH_NAMESPACE;

#endif
