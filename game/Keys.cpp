// Description:
//   Keyboard/Trigger helpers.
//
// Copyright (C) 2007 Frank Becker
//
#include "Keys.hpp"

#include <algorithm>
#include <sstream>

#include "Trace.hpp"
#include "StringUtils.hpp"

using namespace std;

bool Keys::convertStringToTrigger(string& keyname, Trigger& trigger) {
    //    XTRACE();
    if (keyname == "MOTION") {
        trigger.type = eMotionTrigger;
        trigger.data1 = 0;
        trigger.data2 = 0;
        trigger.data3 = 0;
        return true;
    }

    if (keyname.substr(0, 11) == "MOUSEBUTTON") {
        trigger.type = eButtonTrigger;
        if (keyname.length() == 12) {
            trigger.data1 = keyname[11] - '0';
        } else {
            trigger.data1 = 1;  //default to button 1
        }
        //        trigger.data1= atoi( keyname.substr(11,1).c_str());
        trigger.data2 = 0;
        trigger.data3 = 0;
        return true;
    }

    trigger.type = eKeyTrigger;
    unsigned int i;
    for (i = 0; i < SDL_NUM_SCANCODES; i++) {
        if (_symmap[i] == keyname) {
            break;
        }
    }

    if (i == SDL_NUM_SCANCODES) {
        return false;
    }

    trigger.data1 = SDL_GetKeyFromScancode((SDL_Scancode)i);
    trigger.data2 = 0;
    trigger.data3 = i;

    LOG_INFO << "trigger for " << keyname << " " << trigger.data1 << "\n";

    return true;
}

string Keys::convertTriggerToString(const Trigger& trigger) {
    //    XTRACE();
    string triggerName;

    switch (trigger.type) {
        case eKeyTrigger:
            triggerName = _symmap[trigger.data3];  //data3 is scancode
            break;
        case eButtonTrigger: {
            ostringstream ost;
            ost << "MOUSEBUTTON" << trigger.data1;
            triggerName = ost.str();
        } break;
        case eMotionTrigger:
            triggerName = "MOTION";
            break;
        case eUnknownTrigger:
        default:
            triggerName = "UNKNOWN_";
            break;
    }

    return triggerName;
}

Keys::Keys(void) {
    XTRACE();
}

void Keys::init(void) {
#if 0
    for( int i=0; i<SDL_NUM_SCANCODES; i++)
    {
        SDL_Keycode key = SDL_GetKeyFromScancode((SDL_Scancode)i);
        std::string keyName = SDL_GetKeyName(key);
        LOG_INFO << "keyname -> " << key << ":" << keyName << "\n";

        _symmap[i] = StringUtils(keyName).toUpper();
    }
#endif
    _symmap[SDL_SCANCODE_UNKNOWN] = "UNKNOWN";
    _symmap[SDL_SCANCODE_BACKSPACE] = "BACKSPACE";
    _symmap[SDL_SCANCODE_TAB] = "TAB";
    _symmap[SDL_SCANCODE_CLEAR] = "CLEAR";
    _symmap[SDL_SCANCODE_RETURN] = "RETURN";
    _symmap[SDL_SCANCODE_PAUSE] = "PAUSE";
    _symmap[SDL_SCANCODE_ESCAPE] = "ESCAPE";
    _symmap[SDL_SCANCODE_SPACE] = "SPACE";
    _symmap[SDL_SCANCODE_COMMA] = "COMMA";
    _symmap[SDL_SCANCODE_MINUS] = "MINUS";
    _symmap[SDL_SCANCODE_PERIOD] = "PERIOD";
    _symmap[SDL_SCANCODE_SLASH] = "SLASH";
    _symmap[SDL_SCANCODE_0] = "0";
    _symmap[SDL_SCANCODE_1] = "1";
    _symmap[SDL_SCANCODE_2] = "2";
    _symmap[SDL_SCANCODE_3] = "3";
    _symmap[SDL_SCANCODE_4] = "4";
    _symmap[SDL_SCANCODE_5] = "5";
    _symmap[SDL_SCANCODE_6] = "6";
    _symmap[SDL_SCANCODE_7] = "7";
    _symmap[SDL_SCANCODE_8] = "8";
    _symmap[SDL_SCANCODE_9] = "9";
    _symmap[SDL_SCANCODE_SEMICOLON] = "SEMICOLON";
    _symmap[SDL_SCANCODE_EQUALS] = "EQUALS";
    _symmap[SDL_SCANCODE_LEFTBRACKET] = "LEFTBRACKET";
    _symmap[SDL_SCANCODE_BACKSLASH] = "BACKSLASH";
    _symmap[SDL_SCANCODE_RIGHTBRACKET] = "RIGHTBRACKET";
    _symmap[SDL_SCANCODE_A] = "A";
    _symmap[SDL_SCANCODE_B] = "B";
    _symmap[SDL_SCANCODE_C] = "C";
    _symmap[SDL_SCANCODE_D] = "D";
    _symmap[SDL_SCANCODE_E] = "E";
    _symmap[SDL_SCANCODE_F] = "F";
    _symmap[SDL_SCANCODE_G] = "G";
    _symmap[SDL_SCANCODE_H] = "H";
    _symmap[SDL_SCANCODE_I] = "I";
    _symmap[SDL_SCANCODE_J] = "J";
    _symmap[SDL_SCANCODE_K] = "K";
    _symmap[SDL_SCANCODE_L] = "L";
    _symmap[SDL_SCANCODE_M] = "M";
    _symmap[SDL_SCANCODE_N] = "M";
    _symmap[SDL_SCANCODE_O] = "O";
    _symmap[SDL_SCANCODE_P] = "P";
    _symmap[SDL_SCANCODE_Q] = "Q";
    _symmap[SDL_SCANCODE_R] = "R";
    _symmap[SDL_SCANCODE_S] = "S";
    _symmap[SDL_SCANCODE_T] = "T";
    _symmap[SDL_SCANCODE_U] = "U";
    _symmap[SDL_SCANCODE_V] = "V";
    _symmap[SDL_SCANCODE_W] = "W";
    _symmap[SDL_SCANCODE_X] = "X";
    _symmap[SDL_SCANCODE_Y] = "Y";
    _symmap[SDL_SCANCODE_Z] = "Z";
    _symmap[SDL_SCANCODE_DELETE] = "DELETE";
    _symmap[SDL_SCANCODE_KP_0] = "KP0";
    _symmap[SDL_SCANCODE_KP_1] = "KP1";
    _symmap[SDL_SCANCODE_KP_2] = "KP2";
    _symmap[SDL_SCANCODE_KP_3] = "KP3";
    _symmap[SDL_SCANCODE_KP_4] = "KP4";
    _symmap[SDL_SCANCODE_KP_5] = "KP5";
    _symmap[SDL_SCANCODE_KP_6] = "KP6";
    _symmap[SDL_SCANCODE_KP_7] = "KP7";
    _symmap[SDL_SCANCODE_KP_8] = "KP8";
    _symmap[SDL_SCANCODE_KP_9] = "KP9";
    _symmap[SDL_SCANCODE_KP_PERIOD] = "KP_PERIOD";
    _symmap[SDL_SCANCODE_KP_DIVIDE] = "KP_DIVIDE";
    _symmap[SDL_SCANCODE_KP_MULTIPLY] = "KP_MULTIPLY";
    _symmap[SDL_SCANCODE_KP_MINUS] = "KP_MINUS";
    _symmap[SDL_SCANCODE_KP_PLUS] = "KP_PLUS";
    _symmap[SDL_SCANCODE_KP_ENTER] = "KP_ENTER";
    _symmap[SDL_SCANCODE_KP_EQUALS] = "KP_EQUALS";
    _symmap[SDL_SCANCODE_UP] = "UP";
    _symmap[SDL_SCANCODE_DOWN] = "DOWN";
    _symmap[SDL_SCANCODE_RIGHT] = "RIGHT";
    _symmap[SDL_SCANCODE_LEFT] = "LEFT";
    _symmap[SDL_SCANCODE_INSERT] = "INSERT";
    _symmap[SDL_SCANCODE_HOME] = "HOME";
    _symmap[SDL_SCANCODE_END] = "END";
    _symmap[SDL_SCANCODE_PAGEUP] = "PAGEUP";
    _symmap[SDL_SCANCODE_PAGEDOWN] = "PAGEDOWN";
    _symmap[SDL_SCANCODE_F1] = "F1";
    _symmap[SDL_SCANCODE_F2] = "F2";
    _symmap[SDL_SCANCODE_F3] = "F3";
    _symmap[SDL_SCANCODE_F4] = "F4";
    _symmap[SDL_SCANCODE_F5] = "F5";
    _symmap[SDL_SCANCODE_F6] = "F6";
    _symmap[SDL_SCANCODE_F7] = "F7";
    _symmap[SDL_SCANCODE_F8] = "F8";
    _symmap[SDL_SCANCODE_F9] = "F9";
    _symmap[SDL_SCANCODE_F10] = "F10";
    _symmap[SDL_SCANCODE_F11] = "F11";
    _symmap[SDL_SCANCODE_F12] = "F12";
    _symmap[SDL_SCANCODE_F13] = "F13";
    _symmap[SDL_SCANCODE_F14] = "F14";
    _symmap[SDL_SCANCODE_F15] = "F15";
    _symmap[SDL_SCANCODE_CAPSLOCK] = "CAPSLOCK";
    _symmap[SDL_SCANCODE_RSHIFT] = "RSHIFT";
    _symmap[SDL_SCANCODE_LSHIFT] = "LSHIFT";
    _symmap[SDL_SCANCODE_RCTRL] = "RCTRL";
    _symmap[SDL_SCANCODE_LCTRL] = "LCTRL";
    _symmap[SDL_SCANCODE_RALT] = "RALT";
    _symmap[SDL_SCANCODE_LALT] = "LALT";
    _symmap[SDL_SCANCODE_MODE] = "MODE";
    _symmap[SDL_SCANCODE_HELP] = "HELP";
    _symmap[SDL_SCANCODE_SYSREQ] = "SYSREQ";
    _symmap[SDL_SCANCODE_MENU] = "MENU";
    _symmap[SDL_SCANCODE_POWER] = "POWER";
}

Keys::~Keys() {
    XTRACE();
}
