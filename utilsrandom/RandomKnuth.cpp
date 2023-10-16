// Description:
//   Fast (pseudo) random number generator.
//
// Copyright (C) 2011 Frank Becker
//
#include "RandomKnuth.hpp"

static unsigned int defaultRandomNumbers[64] = {
    423497850, 293874509, 872340985, 720349875, 29834709,  389735263, 249879782, 135345234, 852938745, 982697526,
    924387349, 824624386, 423645625, 602586386, 478557463, 101398630, 987197471, 51798980,  798517092, 309717542,
    523778179, 801324079, 705476523, 976215378, 913405967, 78231980,  357243764, 320743262, 306743789, 420970738,
    437809432, 784284784, 789017893, 478248247, 437624367, 436984342, 362436234, 122433436, 266802430, 984360250,
    346230523, 487023872, 436876234, 987625978, 632874675, 432689432, 872398243, 698259634, 62398734,  652986324,
    439084327, 623462397, 823486567, 243729784, 376576237, 439082364, 624397825, 398749234, 862354987, 525268234,
    342098742, 397843269, 234983498, 986538974};

RandomKnuth::RandomKnuth(void) {
    reset(29, 63);
}

RandomKnuth::RandomKnuth(unsigned int seed1, unsigned int seed2) {
    reset(seed1, seed2);
}

void RandomKnuth::reset(unsigned int seed1, unsigned int seed2) {
    _seed1 = seed1;
    _seed2 = seed2;

    for (int i = 0; i < 64; i++) {
        _randomNumbers[i] = defaultRandomNumbers[i];
    }
}

unsigned int RandomKnuth::random(void) {
    unsigned int ret = _randomNumbers[_seed1];
    ret += _randomNumbers[_seed2];
    _randomNumbers[_seed2] = ret;
    _seed1 = (_seed1 + 1) & 63;
    _seed2 = (_seed2 + 1) & 63;
    return ret & _RAND_MAX;
}

float RandomKnuth::rangef0_1(void) {
    return (float)random() / (float)(_RAND_MAX);
}

double RandomKnuth::ranged0_1(void) {
    return (double)random() / (double)(_RAND_MAX);
}
