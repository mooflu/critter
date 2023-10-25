#pragma once
// Description:
//   Kirkpatrick, S., and E. Stoll, 1981; A Very Fast Shift-Register
//   Sequence Random Number Generator, Journal of Computational Physics, V. 40.
//
// Copyright (C) 2011 Frank Becker
//

#include "RandomI.hpp"

class R250 : public RandomI {
    static const unsigned int _RAND_MAX = 0x7fffffff;

public:
    R250(void);
    R250(unsigned int seed);
    virtual ~R250(){};

    unsigned int random(void);
    float rangef0_1(void);
    double ranged0_1(void);

    void reset(unsigned int seed);

private:
    unsigned int _index;
    unsigned int _randomNumbers[250];
    unsigned int _seed;
};
