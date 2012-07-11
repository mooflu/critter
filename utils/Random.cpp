// Description:
//   Fast (pseudo) random number generator.
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
#include "Random.hpp"

#ifdef USE_SIMPLE_RANDOM
unsigned int Random::randomNumbers[ 64]= 
{
    423497850,293874509,872340985,720349875, 29834709,
    389735263,249879782,135345234,852938745,982697526,
    924387349,824624386,423645625,602586386,478557463,
    101398630,987197471, 51798980,798517092,309717542,
    523778179,801324079,705476523,976215378,913405967,
     78231980,357243764,320743262,306743789,420970738,
    437809432,784284784,789017893,478248247,437624367,
    436984342,362436234,122433436,266802430,984360250,
    346230523,487023872,436876234,987625978,632874675,
    432689432,872398243,698259634, 62398734,652986324,
    439084327,623462397,823486567,243729784,376576237,
    439082364,624397825,398749234,862354987,525268234,
    342098742,397843269,234983498,986538974
};

unsigned int Random::seed1 = 29;
unsigned int Random::seed2 = 63;
#endif
