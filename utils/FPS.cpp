// Description:
//   Helpers to count Frames Per Second.
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
#include "FPS.hpp"

#include <stdio.h>
#include "Timer.hpp"

WPP::WPP( float period):
    _cpp(0.0),
    _period(period),
    _oldTime(0.0),
    _count(0)
{
}
    
void WPP::Update( void)
{
    float newTime = Timer::getTime();
    _count++;

    if( (newTime-_oldTime) > _period)
    {
	_cpp = ((float)_count)/(newTime-_oldTime);
	_count = 0;
	_oldTime = newTime;
    }
}

WPP FPS::_wpp(1.0);
char FPS::_fpsString[10];

const char *FPS::GetFPSString( void)
{
    sprintf( _fpsString, "%3.1f", FPS::GetFPS());
    return _fpsString;
}
