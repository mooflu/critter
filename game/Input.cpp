// Description:
//   Input subsystem.
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
#include "Input.hpp"

#include <math.h>
#include "SDL/SDL.h"

#include "Trace.hpp"
#include "Config.hpp"
#include "Callback.hpp"
#include "GameState.hpp"
#include "FindHash.hpp"
#include "CallbackManager.hpp"
#include "Tokenizer.hpp"
#include "Value.hpp"

#ifdef __APPLE__
#define MAIN_MODIFIER KMOD_META
#else
#define MAIN_MODIFIER KMOD_CTRL
#endif

using namespace std;

Input::Input( void):
    _bindMode( false),
    _action(""),
    _callbackManager(),
    _memoryDX(0.0),
    _memoryDY(0.0),
    _valDX(0.0),
    _valDY(0.0),
    _dampVal(0.0),
    _sensitivity(0.5),
    _interceptor(0)
{
    XTRACE();
}

Input::~Input()
{
    XTRACE();
    _callbackMap.clear();

    ATMap::iterator i;
    for( i=_actionTriggerMap.begin(); i!=_actionTriggerMap.end() ;i++)
    {
        delete i->second;
    }
}

bool Input::init( void)
{
    XTRACE();
    LOG_INFO << "Initializing Input..." << endl;

    updateMouseSettings();
    LOG_INFO << "Mouse smoothing: " << _dampVal << endl;
    LOG_INFO << "Mouse sensitivity: " << _sensitivity << endl;

    SDL_EnableKeyRepeat( 300,200);

    _callbackManager.init();

    LOG_INFO << "Input OK." << endl;
    return true;
}

void Input::updateMouseSettings( void)
{
    ConfigS::instance()->getFloat( "mouseSmooth", _dampVal);
    if( (_dampVal<0.0) || (_dampVal>0.999))
    {
        _dampVal = 0.0f;
    }

    ConfigS::instance()->getFloat( "mouseSensitivity", _sensitivity);
    if( (_sensitivity<0.01) || (_sensitivity>1.0))
    {
        _sensitivity = 0.1f;
    }
}

// Returns false, if there are no more events available.
bool Input::tryGetTrigger( Trigger &trigger, bool &isDown)
{
//    XTRACE();
    isDown = false;

    SDL_Event event;
    if( !SDL_PollEvent( &event))
    {
        return false;
    }

    switch( event.type ) 
    {
	case SDL_KEYDOWN:
	    isDown = true;
	    if( (event.key.keysym.sym == SDLK_BACKQUOTE) &&
                (event.key.keysym.mod & KMOD_SHIFT))                
	    {
		LOG_INFO << "Resolution reset..." << endl;
                Value *w = new Value( 800);
                ConfigS::instance()->updateKeyword( "width", w);
                Value *h = new Value( 600);
                ConfigS::instance()->updateKeyword( "height", h);
                
		trigger.type = eUnknownTrigger;
		break;
	    }
            if( (event.key.keysym.sym == SDLK_g) &&
                (event.key.keysym.mod & MAIN_MODIFIER))                
            {
		SDL_GrabMode gmode = SDL_WM_GrabInput( SDL_GRAB_QUERY);
		if( gmode == SDL_GRAB_OFF )
		    SDL_WM_GrabInput( SDL_GRAB_ON);
		else if ( gmode == SDL_GRAB_ON )
		    SDL_WM_GrabInput( SDL_GRAB_OFF);
	    }
            if( (event.key.keysym.sym == SDLK_f) &&
                (event.key.keysym.mod & MAIN_MODIFIER))                
            {
                bool fullscreen = false;
                ConfigS::instance()->getBoolean( "fullscreen", fullscreen);

                Value *w = new Value( ! fullscreen);
                ConfigS::instance()->updateKeyword( "fullscreen", w);
            }
                    
#ifndef NO_QUICK_EXIT
	    if( (event.key.keysym.sym == SDLK_BACKQUOTE) 
                || ((event.key.keysym.sym == SDLK_q) && (event.key.keysym.mod & MAIN_MODIFIER))
                )
	    {
		GameState::isAlive = false;
		LOG_WARNING << "Quick Exit invoked..." << endl;

		trigger.type = eUnknownTrigger;
		break;
	    }
#endif
	    //fall through

	case SDL_KEYUP:
	    trigger.type = eKeyTrigger;
	    trigger.data1 = event.key.keysym.sym;
	    trigger.data2 = event.key.keysym.mod;
	    trigger.data3 = event.key.keysym.unicode;
	    break;

	case SDL_MOUSEBUTTONDOWN:
	    isDown = true;
	    //fall through

	case SDL_MOUSEBUTTONUP:
	    trigger.type = eButtonTrigger;
	    trigger.data1 = event.button.button;
	    trigger.data2 = 0;
	    trigger.data3 = 0;
	    break;

	case SDL_MOUSEMOTION:
	    trigger.type = eUnknownTrigger;
	    _valDX = event.motion.xrel*_sensitivity;
	    _valDY = -event.motion.yrel*_sensitivity;
	    break;

	case SDL_QUIT:
	    GameState::isAlive = false;
	    break;

	default:
	    trigger.type = eUnknownTrigger;
            break;
    }

    return true;
}

bool Input::update( void)
{
//    XTRACE();
    bool isDown;
    Trigger trigger;

    static float nextTime = Timer::getTime()+0.5f;
    float thisTime = Timer::getTime();
    if( thisTime > nextTime)
    {
	updateMouseSettings();
	nextTime = thisTime+0.5f;
    }

    _valDX = 0.0f;
    _valDY = 0.0f;

    while( tryGetTrigger( trigger, isDown))
    {
        if( trigger.type == eUnknownTrigger)
        {
            //for unkown trigger we don't need to do a lookup
            continue;
        }
        
        //Note: motion triggers can't be bound
        if( _bindMode && isDown && _action.size() && (trigger.type!=eMotionTrigger))
        {
            bool validBind = true;
            switch( trigger.type)
            {
                case eKeyTrigger:
                    switch( trigger.data1)
                    {
                        case SDLK_ESCAPE:
                            validBind = false;
                            break;
                        default:
                            break;
                    }
                    break;
                    
                case eButtonTrigger:
                    break;
                    
                default:
                    break;
            }
            if( validBind)
            {
                //LOG_INFO << "Trying to get new bind for " << _action << "\n";

                Callback *cb2 = findHash( trigger, _callbackMap);
                if( cb2)
                {
                    _actionTriggerMap.erase( cb2->getActionName());
                }                    
                
                Trigger *t = findHash( _action, _actionTriggerMap);
                Callback *cb = _callbackManager.getCallback( _action);
                
                if( t)
                {
                    //get rid of previous trigger mapping
                    _actionTriggerMap.erase( _action);
                    _callbackMap.erase( *t);
                }
                else
                {
                    t = new Trigger;
                }
                
                //update trigger with new settings
                *t = trigger;
                _actionTriggerMap[ _action] = t;
                _callbackMap[ *t] = cb;
            }
            
            //go back to normal mode
	    _bindMode = false;
            continue;
        }
    
	if( _interceptor)
	{
	    //feed trigger to interceptor instead of normal callback mechanism
	    _interceptor->input( trigger, isDown);
	    continue;
	}
      
        if( !_bindMode)
	{
	    //find callback for this trigger
	    //i.e. the action bound to this key
	    Callback * cb = findHash( trigger, _callbackMap);
	    if( cb)
	    {
//		LOG_INFO << "Callback for [" << cb->getActionName() << "]" << endl;
		cb->performAction( trigger, isDown);
	    }
	}
        else if( !_action.size())
        {
            LOG_ERROR << "Input is in bind mode, but no action" << endl;
	    _bindMode = false;
        }
    }
    _valDX = feedbackFilter( _valDX, _dampVal, _memoryDX);
    _valDY = feedbackFilter( _valDY, _dampVal, _memoryDY);

    if( (fabs(_valDX)>1.0e-10) || (fabs(_valDY)>1.0e-10))
    {
	trigger.type = eMotionTrigger;
	trigger.fData1 = _valDX; 
	trigger.fData2 = _valDY;

	if( _interceptor)
	{
	    //feed trigger to interceptor instead of normal callback mechanism
	    _interceptor->input( trigger, true);
	}
	else
	{
	    Callback * cb = findHash( trigger, _callbackMap);
	    if( cb)
	    {
//	        LOG_INFO << "Callback for [" << cb->getActionName() << "]" << endl;
		cb->performAction( trigger, isDown);
	    }
	}
    }

    return true;
}

void Input::handleLine( const string line)
{
//    XTRACE();
    Tokenizer  t( line);
    string bindKeyword = t.next();
    if( bindKeyword != "bind") return;

    string action = t.next();
    string keyname = t.next();

//    LOG_INFO << "action [" << action << "], "
//             << "keyname [" << keyname << "]" << endl;

    Trigger *trigger = new Trigger;
    if( _keys.convertStringToTrigger( keyname, *trigger))
    {
	_actionTriggerMap[ action] = trigger;
    }
}

void Input::save( ofstream &outfile)
{
    XTRACE();
    outfile << "# --- Binding section --- " << endl;

    hash_map< Trigger, Callback*, hash<Trigger> >::const_iterator ci;
    for( ci=_callbackMap.begin(); ci!=_callbackMap.end(); ci++)
    {
        outfile << "bind " 
                << ci->second->getActionName() << " "
                << _keys.convertTriggerToString( ci->first)
                << endl;
    }
}

void Input::addCallback( Callback *cb)
{
    if( cb)
    {
        _callbackManager.addCallback( cb);
        
	Trigger *t = findHash( cb->getActionName(), _actionTriggerMap);
        if( !t)
        {
            //add default
            t = new Trigger;
            if( _keys.convertStringToTrigger( cb->getDefaultTriggerName(), *t))
            {
                Callback * cbAlreadyThere = findHash( *t, _callbackMap);
                if( cbAlreadyThere) 
                {
                    delete t;
                    return;
                }                
                _actionTriggerMap[ cb->getActionName()] = t;
            }            
        }
        
	if( t)
	{
	    bind( *t, cb);
	}
    }
}

std::string Input::getTriggerName( std::string &action)
{
    Trigger *t = findHash( action, _actionTriggerMap);
    if( t)
    {
        return _keys.convertTriggerToString( *t);
    }
    return "Not assigned!";
}


void Input::bind( Trigger &trigger, Callback *callback)
{
    XTRACE();
    Callback * cb = findHash( trigger, _callbackMap);
    if( cb)
    {
        LOG_WARNING << "Removing old binding" << endl;
        //remove previous callback...
        _callbackMap.erase( trigger);
    }

    LOG_INFO << "Creating binding for " << callback->getActionName() 
             << " - " << trigger.type << ":" << trigger.data1 << endl;
    _callbackMap[ trigger] = callback;
}
