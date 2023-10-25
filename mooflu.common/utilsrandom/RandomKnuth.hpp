#pragma once
// Description:
//   Fast (pseudo) random number generator.
//   The "simple" random generator is usually random enough for game type
//   applications and requires just adds and ands. (see Knuth)
//
// Copyright (C) 2011 Frank Becker
//

#include "RandomI.hpp"

class RandomKnuth : public RandomI {
    static const unsigned int _RAND_MAX = 0x7fffffff;

public:
    RandomKnuth(void);
    RandomKnuth(unsigned int seed1, unsigned int seed2);

    virtual ~RandomKnuth() {}

    unsigned int random(void);
    float rangef0_1(void);
    double ranged0_1(void);

private:
    void reset(unsigned int seed1, unsigned int seed2);

    unsigned int _seed1;
    unsigned int _seed2;
    unsigned int _randomNumbers[64];
};
