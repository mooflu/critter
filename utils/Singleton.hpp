#pragma once
// Description:
//   Template for singleton pattern.
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

template <class TYPE>
class Singleton
{
public:
    static TYPE *instance (void)
    {
        if( !_instance)
        {
            _instance = new TYPE();
        }
        return _instance;
    }

    static void cleanup( void)
    {
        delete _instance;
        _instance = 0;
    }

private:
    Singleton (void);
    static TYPE *_instance;
};

template <class TYPE>
TYPE * Singleton<TYPE>::_instance = 0;
