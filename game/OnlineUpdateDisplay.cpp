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
#include <Trace.hpp>
#include <FontManager.hpp>
#include <Video.hpp>
#include <GameState.hpp>
#include <Config.hpp>

#include <Constants.hpp>
#include <OnlineUpdate.hpp>
#include <OnlineUpdateDisplay.hpp>

OnlineUpdateDisplay::OnlineUpdateDisplay( void):
  _posX(VIDEO_ORTHO_WIDTH),
  _delay(10),
  _updateIsReady(false)
{
    _currentMessage = _messages.begin();
}

OnlineUpdateDisplay::~OnlineUpdateDisplay()
{
}

void OnlineUpdateDisplay::update( void)
{
    bool onlineCheck = false;
    ConfigS::instance()->getBoolean("onlineCheck", onlineCheck);
    if( !onlineCheck) return;

    GLBitmapFont &fontWhite = 
	*(FontManagerS::instance()->getFont( "bitmaps/arial-small"));

    if( ! _updateIsReady)
    {
	OnlineUpdate &ou = *OnlineUpdateS::instance();
	if( ou.getUpdateStatus() == OnlineUpdate::eSuccess)
	{
	    list<OnlineUpdate::NewsItem*> &newsItemList = ou.getNewsItems();
	    list<OnlineUpdate::NewsItem*>::iterator i;
	    for( i=newsItemList.begin(); i!=newsItemList.end(); i++)
	    {
		MessageInfo mi;
		mi.message = (*i)->title + ": " + (*i)->text + " (" + (*i)->date + ")";
		mi.r = (*i)->r;
		mi.g = (*i)->g;
		mi.b = (*i)->b;
		_messages.insert( _messages.end(), mi);
	    }

	    if(!ou.isLatest())
	    {
		MessageInfo mi;
		mi.message = "A new version of " + GAMETITLE + " is available! Latest version is " + ou.getLatestVersion() + ". " + ou.getLatestVersionText();
		mi.r = 1.0;
		mi.g = 0.2;
		mi.b = 0.0;
		_messages.insert( _messages.begin(), mi);
	    }
	    _currentMessage = _messages.begin();
	    _updateIsReady = true;

	    MessageInfo mi = (*_currentMessage);
	    _currentTextLength = fontWhite.GetWidth( mi.message.c_str(), 0.6);
	}
    }

    if( _currentMessage == _messages.end()) return;

    _prevPosX = _posX;
    if( _posX > -_currentTextLength)
    {
	_posX-=5.0;
    }
    else if( _delay > 0 )
    {
	_delay--;
	if( _delay == 0)
	{
	    _posX=VIDEO_ORTHO_WIDTH;
	    _prevPosX = _posX;
	    _delay=10; //00;
	    _currentMessage++;
	    if( _currentMessage == _messages.end())
		_currentMessage = _messages.begin();

	    MessageInfo mi = (*_currentMessage);
	    _currentTextLength = fontWhite.GetWidth( mi.message.c_str(), 0.6);
	}
    }
}

void OnlineUpdateDisplay::draw( void)
{
    bool onlineCheck = false;
    ConfigS::instance()->getBoolean("onlineCheck", onlineCheck);
    if( !onlineCheck) return;

    if( !_updateIsReady) return;

    if( _currentMessage == _messages.end()) return;

    MessageInfo mi= (*_currentMessage);

    GLBitmapFont &fontWhite = 
	*(FontManagerS::instance()->getFont( "bitmaps/arial-small"));
    glColor4f(mi.r, mi.g, mi.b, 0.7);

    glPushMatrix();
    float posX = _prevPosX+(_posX-_prevPosX)*GameState::frameFractionOther;
    fontWhite.DrawString( mi.message.c_str(), posX, 25, 0.6, 0.6);
    glPopMatrix();
}
#endif
