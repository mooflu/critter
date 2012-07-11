#pragma once
#ifdef USE_ONLINE_UPDATE
// Description:
//   OpenGL display for online update.
//
// Copyright (C) 2005 Frank Becker
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
#include <list>
#include <string>

using namespace std;

class OnlineUpdateDisplay
{
    struct MessageInfo
    {
	string message;
	float r;
	float g;
	float b;
    };
public:
    OnlineUpdateDisplay( void);
    ~OnlineUpdateDisplay();

    void update( void);
    void draw( void);

private:
    float _posX;
    float _prevPosX;
    int _delay;
    float _currentTextLength;

    bool _updateIsReady;
    list<MessageInfo> _messages;
    list<MessageInfo>::iterator _currentMessage;
};
#endif
