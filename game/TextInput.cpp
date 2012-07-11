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

TextInput::TextInput( unsigned int maxLength):
    _maxLen(maxLength),
    _line(""),
    _isOn( false)
{
    XTRACE();
}

TextInput::~TextInput()
{
    XTRACE();
}

void TextInput::input( const Trigger &trigger, const bool &isDown)
{
    XTRACE();
    Trigger t = trigger;
    if( isDown)
    {
	switch( trigger.type)
	{
	    case eKeyTrigger:
	        switch( trigger.data1)
		{
		    case SDLK_ESCAPE:
		    case SDLK_RETURN:
		        turnOff();
			break;

		    case SDLK_DELETE:
		    case SDLK_BACKSPACE:
		        if( _line.length() > 0)
			{
			    _line.erase( _line.length()-1, 1);
			}
			break;

		    default:
			break;
		}

	        if( (_line.length() <= _maxLen) &&
		    (trigger.data1 >=  SDLK_SPACE) &&
		    (trigger.data1 <= SDLK_z))
		{
		    char c;
		    //Accept simple unicode chars. Ie. ASCII
		    if( (trigger.data3 != 0) && ((trigger.data3 & ~0xff) == 0))
		    {
			c=(char)(trigger.data3 & 0xff);
		    }
		    else
		    {
			//Poor boy's version of ASCII...
			c=(char)tolower(trigger.data1);
			if( trigger.data2 & KMOD_SHIFT)
			{
			    c = toupper(c);
			}
		    }
		    _line += c;
		}
                break;

	    default:
                break;
	}
    }
}

void TextInput::turnOn( void)
{
    XTRACE();
    SDL_EnableUNICODE( true);
    InputS::instance()->enableInterceptor( this);
    _isOn = true;
}

void TextInput::turnOff( void)
{
    XTRACE();

    InputS::instance()->disableInterceptor();
    SDL_EnableUNICODE( false);

    //pass escape to previous input handling...
    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_ESCAPE;
    SDL_PushEvent( &event);

    _isOn = false;
}
