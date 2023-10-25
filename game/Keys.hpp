#pragma once
// Description:
//   Keyboard/Trigger helpers.
//
// Copyright (C) 2007 Frank Becker
//
#include <string>

#include "SDL.h"

#include "Trigger.hpp"

class Keys {
public:
    Keys(void);
    ~Keys();
    void init();

    std::string convertTriggerToString(const Trigger& trigger);
    bool convertStringToTrigger(std::string& triggerStr, Trigger& trigger);

private:
    Keys(const Keys&);
    Keys& operator=(const Keys&);

    std::string _symmap[SDL_NUM_SCANCODES];
};
