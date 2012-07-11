#pragma once
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

//General counter
class WPP
{
public:
    WPP( float period = 1.0f);
    void Update( void);
    float GetCountPerPeriod( void){ return _cpp;}

private:
    WPP( const WPP&);
    WPP &operator=(const WPP&);

    float _cpp;
    float _period;
    float _oldTime;
    int _count;
};

//Special Frame-per-second counter
//Note: static
class FPS
{
public:
    static void Update( void){ _wpp.Update();}
    static float GetFPS( void){ return _wpp.GetCountPerPeriod();}
    static const char *GetFPSString( void);

private:
    ~FPS();
    FPS( void);
    FPS( const FPS&);
    FPS &operator=(const FPS&);

    static WPP _wpp;
    static char _fpsString[10];
};
