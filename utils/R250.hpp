#pragma once
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

class R250
{
    static const unsigned int _RAND_MAX=0x7fffffff;
public:
    R250( void);
    ~R250();

    void reset( unsigned int seed = 0);
    unsigned int getSeed( void) { return _seed; }

    unsigned int random( void)
    {
	if(_index >= 249)
	    _index = 0;
	else
	    _index++;

	unsigned int i = (_index>=147)? _index-147 : _index+103;
	_randomNumbers[_index] ^= _randomNumbers[i];

	return (_randomNumbers[_index]&_RAND_MAX);
    }

    float rangef0_1( void)
    {
        return (float)random() / (float)(_RAND_MAX);
    }
    double ranged0_1( void)
    {
        return (double)random() / (double)(_RAND_MAX);
    }
    unsigned int integer( unsigned int range)
    {
        return random()%range;
    }

private:
    unsigned int randomize( void);

    unsigned int _index;
    unsigned int _randomNumbers[ 250];
    unsigned int _seed;
};
