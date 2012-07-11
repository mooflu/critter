#pragma once
// Description:
//   Helpers to get time.
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

#ifdef TIMER_USE_GETTIMEOFDAY
#include "gettimeofday.hpp"
#else
#include "SDL/SDL.h"
#endif

class Timer
{
public:
#ifdef TIMER_USE_GETTIMEOFDAY
    static float getTime( void)
    {
        struct timeval tv;
        static bool initialized = false;
        static int startSeconds = 0;
        static int startUSeconds = 0;

        gettimeofday(&tv, 0);
        
        if( ! initialized)
        {
	    startSeconds = tv.tv_sec;
            startUSeconds = tv.tv_usec;
            initialized = true;
        }
        int msecs = (tv.tv_sec-startSeconds)*1000 + (tv.tv_usec-startUSeconds)/1000;
        return((float)msecs/1000.0f);
    } 
#else
    static float getTime( void)
    {
        return( (float)SDL_GetTicks()/1000.0f);
    }

    static void sleep( float sleep)
    {
        Uint32 ms = (Uint32)(sleep*1000.0);
        SDL_Delay( ms);
    }
#endif
};
