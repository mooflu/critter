#pragma once

// Description:
//   Structure for input triggers.
//
// Copyright (C) 2007 Frank Becker
//
#include <string>

enum TriggerTypeEnum {
    eUnknownTrigger,
    eKeyTrigger,
    eButtonTrigger,
    eMotionTrigger,
    eTextInputTrigger,
};

struct Trigger {
    TriggerTypeEnum type;
    int data1;
    int data2;
    int data3;

    //just for mouse smoothing for now...
    float fData1;
    float fData2;

    // text input
    std::string text;

    bool operator==(const Trigger& t) const {
        if ((type == eMotionTrigger) && (type == t.type)) {
            return true;
        }
        //data1 - keycode or mouse button
        //data2 - modifiers (ignore here)
        //data3 - scancode
        return ((type == t.type) && (data1 == t.data1) && (data3 == t.data3));
    }
};
