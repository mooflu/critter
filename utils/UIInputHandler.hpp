#pragma once
// Description:
//   UI input handler interface.
//
// Copyright (C) 2011 Frank Becker
//
#include "Trigger.hpp"

class UIInputHandlerI {
public:
    virtual ~UIInputHandlerI() {}

    virtual bool handle(const Trigger& trigger) = 0;
};
