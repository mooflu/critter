#pragma once
// Description:
//   Fast (pseudo) random number generator.
//   The "simple" random generator is usually random enough for game type
//   applications and requires just adds and ands. (see Knuth)
//   Otherwise "rand()" is used which is quite a bit slower...
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

#define USE_SIMPLE_RANDOM

#ifdef USE_SIMPLE_RANDOM
#else
#include <stdlib.h>
#endif

class Random
{
    static const unsigned int _RAND_MAX=0x7fffffff;
    
#ifdef USE_SIMPLE_RANDOM
private:
    static unsigned int seed1;
    static unsigned int seed2;
    static unsigned int randomNumbers[ 64];
#endif

public:
    static int random( void)
    {
#ifdef USE_SIMPLE_RANDOM
        unsigned int ret = randomNumbers[ seed1];
        ret += randomNumbers[ seed2];
        randomNumbers[ seed2] = ret;
        seed1 = (seed1+1)&63;
        seed2 = (seed2+1)&63;
        return ret&_RAND_MAX;
#else
        return rand();
#endif
    }

    static float rangef0_1( void)
    {
        return (float)random() / (float)(_RAND_MAX);
    }
    static double ranged0_1( void)
    {
        return (double)random() / (double)(_RAND_MAX);
    }
    static unsigned int integer( unsigned int range)
    {
        return random()%range;
    }
};
