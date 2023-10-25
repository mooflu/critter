// Description:
//   Get a line of text.
//
// Copyright (C) 2007 Frank Becker
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
#include "TextInput.hpp"

#include "Trace.hpp"
#include "Input.hpp"

#include <ctype.h>
#include "MenuManager.hpp"

TextInput::TextInput(unsigned int maxLength) :
    _maxLen(maxLength),
    _line(""),
    _isOn(false) {
    XTRACE();
}

TextInput::~TextInput() {
    XTRACE();
}

void TextInput::input(const Trigger& trigger, const bool& isDown) {
    XTRACE();
    switch (trigger.type) {
        case eKeyTrigger:
            if (!isDown) {
                return;
            }

            switch (trigger.data1) {
                case SDLK_ESCAPE:
                case SDLK_RETURN:
                    turnOff();
                    break;

                case SDLK_DELETE:
                case SDLK_BACKSPACE:
                    if (_line.length() > 0) {
                        _line.erase(_line.length() - 1, 1);
                    }
                    break;

                default:
                    break;
            }
            break;

        case eTextInputTrigger:
            if (_line.length() <= _maxLen) {
                //LOG_INFO << "TextInput::input TEXT " << trigger.text << "\n";
                _line += trigger.text;
            }
            break;

        default:
            break;
    }
}

void TextInput::turnOn(void) {
    XTRACE();
    InputS::instance()->enableInterceptor(this);
    SDL_StartTextInput();
    SDL_SetTextInputRect(0);
    _isOn = true;
}

void TextInput::turnOff(void) {
    XTRACE();

    InputS::instance()->disableInterceptor();
    SDL_StopTextInput();

    MenuManagerS::instance()->turnMenuOn();

    _isOn = false;
}
