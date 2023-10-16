#pragma once

class RandomI {
public:
    virtual ~RandomI() {}

    virtual unsigned int random(void) = 0;
    virtual double ranged0_1(void) = 0;

    virtual float rangef0_1(void) { return (float)ranged0_1(); }
};
