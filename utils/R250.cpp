// Description:
//   Kirkpatrick, S., and E. Stoll, 1981; A Very Fast Shift-Register
//   Sequence Random Number Generator, Journal of Computational Physics, V. 40.
//
// Copyright (C) 2003 Frank Becker
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
#include "R250.hpp"

#include <stdlib.h>
#include "gettimeofday.hpp"


R250::R250( void)
{
    reset();
}

R250::~R250()
{ 
}

void R250::reset( unsigned int seed)
{

    if(seed == 0) 
	_seed = randomize();
    else
	_seed = seed;
    srand( _seed);

    for(unsigned int i=0; i<250; i++)
    {
	_randomNumbers[i] = rand();
    }
}

unsigned int R250::randomize( void)
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    return (tv.tv_usec << 12) + tv.tv_sec;
}
