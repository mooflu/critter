// Description:
//   Base for callbacks.
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
#include "Callback.hpp"

#include "Trace.hpp"
#include "Input.hpp"

using namespace std;

Callback::Callback(const string& actionName, const string& defaultTrigger) :
    _actionName(actionName),
    _defaultTrigger(defaultTrigger) {
    XTRACE();
    InputS::instance()->addCallback(this);
}

Callback::~Callback() {
    XTRACE();
}
