#pragma once
// Description:
//   Template to find the matching value for a given key in a hash map.
//
// Copyright (C) 2005 Frank Becker
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
#include "hashMap.hpp"

template< class _KeyT, class _ValT >
_ValT* findHash( 
    _KeyT &trigger, 
	hash_map< _KeyT, _ValT*, hash<_KeyT>, std::equal_to<_KeyT> > & hashMap )
{
    typename hash_map< _KeyT, _ValT*, hash<_KeyT> >::const_iterator ci;
    ci = hashMap.find( trigger);
    if( ci == hashMap.end())
    {
        return 0;
    }

    return ci->second;
}

#ifdef VCPP
template< class _KeyT, class _ValT >
_ValT* findHash( 
    const _KeyT &trigger, 
	hash_map< _KeyT, _ValT*, hash<_KeyT>, std::equal_to<_KeyT> > & hashMap )
{
    typename hash_map< const _KeyT, _ValT*, hash<const _KeyT> >::const_iterator ci;
    ci = hashMap.find( trigger);
    if( ci == hashMap.end())
    {
        return 0;
    }

    return ci->second;
}
#endif
