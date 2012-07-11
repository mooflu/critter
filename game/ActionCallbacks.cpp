// Description:
//   Action callbacks for mouse and keyboard events.
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
#include "SDL/SDL.h"

#include <Trace.hpp>

#include <Input.hpp>
#include <Video.hpp>
#include <Hero.hpp>
#include <Direction.hpp>
#include <Camera.hpp>
#include <MenuManager.hpp>
#include <ActionCallbacks.hpp>
#include <Audio.hpp>

using namespace std;

void MotionAction::performAction( Trigger &trigger, bool /*isDown*/)
{
//    XTRACE();
    switch( GameState::context)
    {
	case Context::eInGame:
	    HeroS::instance()->move( trigger.fData1, trigger.fData2);
	    break;
	case Context::eCameraFlyby:
	    CameraS::instance()->mouseLook( trigger.fData1, trigger.fData2);
	    break;
	default:
	    break;
    }
}

void MotionLeftAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    switch( GameState::context)
    {
	case Context::eInGame:
	    HeroS::instance()->move( Direction::eLeft, isDown);
	    break;
	case Context::eCameraFlyby:
	    CameraS::instance()->move( Direction::eLeft, isDown);
	    break;
	default:
	    break;
    }
}


void MotionRightAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    switch( GameState::context)
    {
	case Context::eInGame:
	    HeroS::instance()->move( Direction::eRight, isDown);
	    break;
	case Context::eCameraFlyby:
	    CameraS::instance()->move( Direction::eRight, isDown);
	    break;
	default:
	    break;
    }
}

void MotionUpAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    switch( GameState::context)
    {
	case Context::eInGame:
	    HeroS::instance()->move( Direction::eUp, isDown);
	    break;
	case Context::eCameraFlyby:
	    CameraS::instance()->move( Direction::eUp, isDown);
	    break;
	default:
	    break;
    }
}

void MotionDownAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    switch( GameState::context)
    {
	case Context::eInGame:
	    HeroS::instance()->move( Direction::eDown, isDown);
	    break;
	case Context::eCameraFlyby:
	    CameraS::instance()->move( Direction::eDown, isDown);
	    break;
	default:
	    break;
    }
}

void WeaponFireAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    switch( GameState::context)
    {
	case Context::eInGame:
	    HeroS::instance()->weaponFire( isDown, _numWeap);
	    break;
	case Context::eCameraFlyby:
	    HeroS::instance()->weaponFire( isDown, _numWeap);
	    break;
	default:
	    break;
    }
}

void SnapshotAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    if( !isDown) return;

    VideoS::instance()->takeSnapshot();
}

void ConfirmAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    if( !isDown) return;

    LOG_INFO << "Yes Sir!" << endl;

#if 0
    switch( GameState::context)
    {
	default:
	    break;
    }
#endif
}

void ChangeContext::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    if( !isDown) return;

    if( GameState::context == Context::eInGame)
    {
	LOG_INFO << "eCameraFlyby..." << endl;
	GameState::context = Context::eCameraFlyby;
	GameState::stopwatch.pause();
    }
    else if( GameState::context == Context::eCameraFlyby)
    {
	LOG_INFO << "eInGame..." << endl;
	GameState::context = Context::eInGame;
	GameState::stopwatch.start();
    }
}

void CritterBoard::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    if( !isDown) return;

//    LOG_INFO << "toggle CritterBoard..." << endl;
    VideoS::instance()->toggleCritterBoard();
}

void PauseGame::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    if( !isDown) return;

    if( GameState::context == Context::ePaused)
    {
	LOG_INFO << "un-pausing..." << endl;
	GameState::context = _prevContext;
	GameState::stopwatch.start();
    }
    else
    {
	LOG_INFO << "pausing..." << endl;
	_prevContext = GameState::context;
	GameState::context = Context::ePaused;
	GameState::stopwatch.pause();
    }
}

void EscapeAction::performAction( Trigger &, bool isDown)
{
//    XTRACE();
    if( !isDown) return;

    switch( GameState::context)
    {
	case Context::eMenu:
	    break;

	default:
//	    LOG_INFO << "Menu mode..." << endl;
            MenuManagerS::instance()->turnMenuOn();
	    break;
    }
}
