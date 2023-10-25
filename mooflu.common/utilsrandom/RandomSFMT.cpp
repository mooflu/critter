// Description:
//   Fast Mersenne Twister pseudorandom number generator based on IEEE 754 format.
//
// Copyright (C) 2011 Frank Becker
//
#include "RandomSFMT.hpp"
#include "RandomDSFMTImpl.h"

RandomSFMT::RandomSFMT(void) :
    _private(0) {
    reset(0);
}

RandomSFMT::RandomSFMT(unsigned int seed) :
    _private(0) {
    reset(seed);
}

RandomSFMT::~RandomSFMT() {
    dsfmt_t* state = (dsfmt_t*)_private;
    delete state;
}

void RandomSFMT::reset(unsigned int seed) {
    if (_private) {
        dsfmt_t* state = (dsfmt_t*)_private;
        delete state;
    }

    dsfmt_t* state = new dsfmt_t;
    _private = state;

    dsfmt_init_gen_rand(state, seed);
}

unsigned int RandomSFMT::random(void) {
    dsfmt_t* state = (dsfmt_t*)_private;
    return dsfmt_genrand_uint32(state);
}

double RandomSFMT::ranged0_1(void) {
    dsfmt_t* state = (dsfmt_t*)_private;
    return dsfmt_genrand_close_open(state);
}
