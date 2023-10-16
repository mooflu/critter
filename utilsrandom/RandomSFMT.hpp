#pragma once
// Description:
//   Fast Mersenne Twister pseudorandom number generator based on IEEE 754 format.
//
// Copyright (C) 2011 Frank Becker
//

#include "RandomI.hpp"

class RandomSFMT : public RandomI {
public:
    RandomSFMT(void);
    RandomSFMT(unsigned int seed);
    virtual ~RandomSFMT();

    unsigned int random(void);
    double ranged0_1(void);

private:
    void reset(unsigned int seed);

    void* _private;
};
