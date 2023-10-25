#pragma once
// Description:
//   Structure for input triggers.
//
// Copyright (C) 2011 Frank Becker
//
#include <string>

enum TriggerTypeEnum {
    eUnknownTrigger = 0,
    eKeyTrigger = 1,
    eButtonTrigger = 2,
    eMotionTrigger = 3,
    eWheelTrigger = 4,
    eTextTrigger = 5
};

struct Trigger {
    TriggerTypeEnum type;
    bool isDown;

    int key;  //key or mouse button
    int modifier;

    //location or distance
    float x;
    float y;

    std::string text;

    Trigger(void) :
        type(eUnknownTrigger),
        isDown(false),
        key(0),
        modifier(0),
        x(0.0f),
        y(0.0f) {}

    Trigger(TriggerTypeEnum triggerType, bool isDown, int key, int mod) :
        type(triggerType),
        isDown(isDown),
        key(key),
        modifier(mod),
        x(0.0f),
        y(0.0f) {}

    Trigger(float x, float y) :
        type(eMotionTrigger),
        isDown(false),
        key(0),
        modifier(0),
        x(x),
        y(y) {}

    Trigger(const std::string& t) :
        type(eTextTrigger),
        isDown(false),
        key(0),
        modifier(0),
        x(0.0f),
        y(0.0f),
        text(t) {}

    bool operator==(const Trigger& t) const {
        switch (type) {
            case eUnknownTrigger:
            case eMotionTrigger:
            case eTextTrigger:
                return (type == t.type);

            case eKeyTrigger:
            case eButtonTrigger:
                return ((type == t.type) && (key == t.key));

            default:
                break;
        }

        return false;
    }
};
