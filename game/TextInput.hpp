#pragma once
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

#include <string>
#include <InterceptorI.hpp>

class TextInput: public InterceptorI
{
public:
    TextInput( unsigned int maxLength=15);
    virtual ~TextInput();

    virtual void input( const Trigger &trigger, const bool &isDown);
    void turnOn( void);
    void turnOff( void);

    bool isOn( void)
    {
	return _isOn;
    }

    const std::string &getText( void)
    {
	return _line;
    }

private:
    TextInput( const TextInput&);
    TextInput &operator=(const TextInput&);

    unsigned int _maxLen;
    std::string _line;
    bool _isOn;
};
