// Description:
//   Kirkpatrick, S., and E. Stoll, 1981; A Very Fast Shift-Register
//   Sequence Random Number Generator, Journal of Computational Physics, V. 40.
//
// Copyright (C) 2011 Frank Becker
//
#include "R250.hpp"

#include <stdlib.h>

R250::R250(void) {
    reset(0);
}

R250::R250(unsigned int seed) {
    reset(seed);
}

void R250::reset(unsigned int seed) {
    _seed = seed;
    srand(_seed);

    for (unsigned int i = 0; i < 250; i++) {
        _randomNumbers[i] = rand();
    }
}

unsigned int R250::random(void) {
    if (_index >= 249) {
        _index = 0;
    } else {
        _index++;
    }

    unsigned int i = (_index >= 147) ? _index - 147 : _index + 103;
    _randomNumbers[_index] ^= _randomNumbers[i];

    return (_randomNumbers[_index] & _RAND_MAX);
}

float R250::rangef0_1(void) {
    return (float)random() / (float)(_RAND_MAX);
}

double R250::ranged0_1(void) {
    return (double)random() / (double)(_RAND_MAX);
}
