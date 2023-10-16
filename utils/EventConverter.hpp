#pragma once
// Description:
//   Input event converter interface.
//
// Copyright (C) 2011 Frank Becker
//
#include "Trigger.hpp"

class EventConverterI {
public:
    virtual ~EventConverterI() {}

    virtual bool getNext(Trigger& trigger) = 0;
};
