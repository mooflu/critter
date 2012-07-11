// Description:c
//   A Selectable
//
// Copyright (C) 2010 Frank Becker
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
#include "Selectable.hpp"

#include "Trace.hpp"
#include "Tokenizer.hpp"
#include "Config.hpp"
#include "Value.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "Audio.hpp"
#include "Input.hpp"
#include "Trigger.hpp"
#include "MenuManager.hpp"
#include "FontManager.hpp"
#include "BitmapManager.hpp"
#include "ScoreKeeper.hpp"

#include "GLee.h"
#include "GLVertexBufferObject.hpp"

#include <algorithm>
using namespace std;

Selectable *Selectable::_active = 0;

Selectable::Selectable( bool enabled, const BoundingBox &r, const string &info):
    _enabled(enabled),
    _inputBox(r),
    _boundingBox(r),
    _info(info),
    _fontWhite(0)
{
    XTRACE();
#ifndef DEMO
    _enabled = true;
#endif
    _fontWhite = FontManagerS::instance()->getFont( "bitmaps/menuWhite");
    if( !_fontWhite)
    {
	LOG_ERROR << "Unable to load menuWhite font." << endl;
    }
}

Selectable::~Selectable() 
{
    XTRACE();
}

void Selectable::draw( const Point2Di &offset)
{
#if 0
    glColor4f( 1.0, 0.2, 0.2, 0.5);
    vec3f v[4] = {
        vec3f( _inputBox.min.x+offset.x, _inputBox.min.y+offset.y, 0 ),
        vec3f( _inputBox.min.x+offset.x, _inputBox.max.y+offset.y, 0 ),
        vec3f( _inputBox.max.x+offset.x, _inputBox.max.y+offset.y, 0 ),
        vec3f( _inputBox.max.x+offset.x, _inputBox.min.y+offset.y, 0 ),
    };
    GLVBO::DrawQuad( GL_QUADS, v);
#endif

    if( _active == this)
    {
	glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
	//_fontWhite->DrawString( _info.c_str(), offset.x+23, offset.y+8, 0.7f, 0.65f);
	_fontWhite->DrawString( _info.c_str(), offset.x+122, offset.y+57, 0.7f, 0.65f);
	//_fontWhite->DrawString( _info.c_str(), 122, 57, 0.7f, 0.65f);
    }
}

void Selectable::updateActive( UserFeedback feedback)
{
    if( (_active != this))
    {
        if( _active) _active->deactivate();
        _active = this;
        MenuManagerS::instance()->Goto( this);
        
        switch( feedback)
        {
            case eBeep:
                AudioS::instance()->playSample( "sounds/beep.wav");
                break;

            case eTick:
                AudioS::instance()->playSample( "sounds/click1.wav");
                break;
                
            case eNoFeedback:
            default:
                break;
        }
    }    
}

//------------------------------------------------------------------------------

EscapeSelectable::EscapeSelectable( bool enabled, const BoundingBox &r, float size):
    Selectable(enabled, r, "Escape"),
    _size(size),
    _icons(0),
    _exitOn(0),
    _exitOff(0)
{
    _icons = BitmapManagerS::instance()->getBitmap( "bitmaps/menuIcons");
    if( !_icons)
    {
	LOG_ERROR << "Unable to load menuIcons." << endl;
    }

    _exitOn = _icons->getIndex( "ExitOn");
    if( _exitOn == -1)
    {
	LOG_ERROR << "ExitOn button not found" << endl;
    }

    _exitOff = _icons->getIndex( "ExitOff");
    if( _exitOff == -1)
    {
	LOG_ERROR << "ExitOff button not found" << endl;
    }
}

void EscapeSelectable::select( void)
{
    MenuManagerS::instance()->Exit( true);
}

void EscapeSelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &/*offset*/)
{
    if( !_enabled) return;

    if( !isDown) return;

    switch( trigger.type)
    {
	case eButtonTrigger:
	    MenuManagerS::instance()->Exit( true);
	    break;

	case eMotionTrigger:
	    this->activate();
	    break;

	default:
	    break;
    }
}

void EscapeSelectable::activate( bool beQuiet)
{
    updateActive( beQuiet ? eNoFeedback : eBeep);
}

void EscapeSelectable::draw( const Point2Di &offset)
{
    Selectable::draw(offset);

    _icons->bind();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    if( (_active == this))
    {
	_icons->Draw( _exitOn, 
	    _boundingBox.min.x + offset.x, 
            _boundingBox.min.y + offset.y, _size, _size);
    }
    else
    {
	_icons->Draw( _exitOff, 
	    _boundingBox.min.x + offset.x, 
            _boundingBox.min.y + offset.y, _size, _size);
    }
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

TextOnlySelectable::TextOnlySelectable( 
    bool enabled, 
    const BoundingBox &rect, 
    const string &text,
    const string &info, 
    bool center,
    float size,
    float red, float green, float blue): 

    Selectable(enabled, rect, info),
    _text(text),
    _fontShadow(0),
    _icons(0),
    r(red),
    g(green),
    b(blue),
    _size(size)
{
    _fontShadow = FontManagerS::instance()->getFont( "bitmaps/menuShadow");
    if( !_fontShadow)
    {
	LOG_ERROR << "Unable to load shadow font." << endl;
    }

    float width  = _fontWhite->GetWidth( _text.c_str(), _size);
    float height = _fontWhite->GetHeight( _size);

    if( center)
    {
	_boundingBox.min.x -= width*0.5f;  
    }
    _boundingBox.max.x = _boundingBox.min.x + width;
    _boundingBox.max.y = _boundingBox.min.y + height;

    _inputBox = _boundingBox;

    _icons = BitmapManagerS::instance()->getBitmap( "bitmaps/menuIcons");
    if( !_icons)
    {
	LOG_ERROR << "Unable to load menuIcons." << endl;
    }
}

void TextOnlySelectable::input( const Trigger &trigger, const bool &/*isDown*/, const Point2Di &/*offset*/)
{
    if( !_enabled) return;
    
    switch( trigger.type)
    {
	case eMotionTrigger:
	    this->activate();
	    break;

	default:
	    break;
    }
}

void TextOnlySelectable::activate( bool /*beQuiet*/)
{
    updateActive( eNoFeedback);
}

void TextOnlySelectable::draw( const Point2Di &offset)
{
    Selectable::draw(offset);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    _fontShadow->DrawString( 
	_text.c_str(), 
	_boundingBox.min.x + offset.x +9*_size,
        _boundingBox.min.y + offset.y -9*_size, 
	_size, _size); 

    if( !_enabled)
        glColor4f(0.5, 0.5, 0.5, 1.0);
    else
        glColor4f(r, g, b, 1.0);
    _fontWhite->DrawString( 
	_text.c_str(), 
	_boundingBox.min.x + offset.x, 
        _boundingBox.min.y + offset.y, 
	_size, _size); 
}

//------------------------------------------------------------------------------

FloatSelectable::FloatSelectable(
    bool enabled,                                  
    const BoundingBox &rect, 
    const string &text,
    const string &info, 
    const string &variable,
    const string &range,
    const string &sliderOffset): 

    TextOnlySelectable(enabled, rect, text, info, false, 0.7f, 1.0f, 1.0f, 1.0f),
    _variable(variable),
    _startX(-1.0f)
{
    Tokenizer t(range);

    _min = (float)atof(t.next().c_str());
    _max = (float)atof(t.next().c_str());

    _sliderOffset = atoi(sliderOffset.c_str());

    float curVal = _min;
    ConfigS::instance()->getFloat( _variable, curVal);
    _xPos = (curVal-_min) * 140.0f / (_max-_min);

    //bounding box for double arrow
    _bRect.min.x = _boundingBox.min.x + 102 + _sliderOffset;
    _bRect.max.x = _bRect.min.x + 20;
    _bRect.min.y = _boundingBox.min.y ;
    _bRect.max.y = _bRect.min.y + 30;

    _slider = _icons->getIndex( "Slider");
    if( _slider == -1)
    {
	LOG_ERROR << "Slider not found" << endl;
    }
    _doubleArrow = _icons->getIndex( "DoubleArrow");
    if( _doubleArrow == -1)
    {
	LOG_ERROR << "DoubleArrow not found" << endl;
    }

    _boundingBox.max.x = _bRect.min.x + _icons->getWidth( _slider)*1.6f;

    _inputBox = _boundingBox;
}

void FloatSelectable::incBy( float delta)
{
    float curVal = _min;
    ConfigS::instance()->getFloat( _variable, curVal);
    curVal += delta;
    Clamp( curVal, _min, _max);
    
    LOG_INFO << "New val = " << curVal << "for " << _variable <<  "\n";
    _xPos = (curVal-_min) * 140.0f / (_max-_min);
    
    Value *v = new Value( curVal);
    ConfigS::instance()->updateKeyword( _variable, v);
    AudioS::instance()->playSample( "sounds/tick1.wav");
}

void FloatSelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &offset)
{
    if( !_enabled) return;
    
    float mouseX = trigger.fData1;
    float mouseY = trigger.fData2;

    switch( trigger.type)
    {
        case eKeyTrigger:
            if( isDown && (_active == this))
            {
                switch( trigger.data1)
                {
                    case SDLK_LEFT:
                        incBy( -0.1);
                        break;
                        
                    case SDLK_RIGHT:
                        incBy( 0.1);
                        break;
                        
                    default:
                        break;
                }                
            }
            break;
            
	case eButtonTrigger:
	    if( isDown)
	    {
		if( (mouseX >= (_bRect.min.x+ offset.x+_xPos)) &&
		    (mouseX <= (_bRect.max.x+ offset.x+_xPos)) &&
		    (mouseY >= (_bRect.min.y+ offset.y)) &&
		    (mouseY <= (_bRect.max.y+ offset.y)) )
		{
		    _startX = mouseX;
		}
	    }
	    else
	    {
		_startX = -1.0f;
		float curVal = _min;
		ConfigS::instance()->getFloat( _variable, curVal);
		curVal = _min + _xPos*(_max-_min)/140.0f;
                //LOG_INFO << "New val = " << curVal << "\n";

		Value *v = new Value( curVal);
		ConfigS::instance()->updateKeyword( _variable, v);
	    }
	    break;

	case eMotionTrigger:
	    if( _startX >= 0.0f)
	    {
		float dx =  mouseX - _startX;
		_startX = mouseX;

		_xPos += dx;
		Clamp( _xPos, 0.0, 140.0);
	    }

	    if( (mouseX >= (_boundingBox.min.x+ offset.x)) &&
		(mouseX <= (_boundingBox.max.x+ offset.x)) &&
		(mouseY >= (_boundingBox.min.y+ offset.y)) &&
		(mouseY <= (_boundingBox.max.y+ offset.y)) )
	    {
		this->activate();
	    }
	    break;

	default:
	    break;
    }
}

void FloatSelectable::activate( bool beQuiet)
{
    updateActive( beQuiet ? eNoFeedback : eBeep);
}

void FloatSelectable::draw( const Point2Di &offset)
{
#if 0
    glColor4f( 0.2, 0.2, 1.0, 0.5);
    vec3f v[4] = {
        vec3f( _bRect.min.x+ offset.x+_xPos, _bRect.min.y+ offset.y, 0),
        vec3f( _bRect.min.x+ offset.x+_xPos, _bRect.max.y+ offset.y, 0),
        vec3f( _bRect.max.x+ offset.x+_xPos, _bRect.max.y+ offset.y, 0),
        vec3f( _bRect.max.x+ offset.x+_xPos, _bRect.min.y+ offset.y, 0),
    };
    GLVBO::DrawQuad( GL_QUADS, v);    
#endif
    TextOnlySelectable::draw(offset);

    _icons->bind();
    if( _active == this)
    {
        if( !_enabled)
            glColor4f(0.7, 0.7, 0.7, 1.0);
        else
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        if( !_enabled)
            glColor4f(0.5, 0.5, 0.5, 1.0);
        else
            glColor4f(1.0, 1.0, 1.0, 1.0);
    }
    
    glEnable(GL_TEXTURE_2D);
    _icons->DrawC( 
        _slider, 
        _boundingBox.min.x + offset.x+180+_sliderOffset, 
        _boundingBox.min.y + offset.y+15, 1.6f, 0.4f);
    _icons->DrawC( 
        _doubleArrow, 
	_boundingBox.min.x + offset.x+112+_sliderOffset+_xPos, 
        _boundingBox.min.y + offset.y+15, 0.8f, 0.8f);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

EnumSelectable::EnumSelectable( 
    bool enabled,
    const BoundingBox &rect, 
    const string &text,
    const string &info, 
    const string &variable,
    const string &values): 

    TextOnlySelectable(enabled, rect, text, info, false, 0.7f, 1.0f, 1.0f, 1.0f),
    _variable(variable)
{
    string val;
    ConfigS::instance()->getString( _variable, val);

    _activeEnum = _enumList.end();
    Tokenizer t(values);
    string s = t.next();
    while( s != "")
    {
	_enumList.insert( _enumList.end(), s);
	if( s == val)
	{
	    _activeEnum = --_enumList.end();
	}
	s = t.next();
    }

    if( _activeEnum == _enumList.end())
    {
	_activeEnum = _enumList.begin();
    }

    _xOff = _boundingBox.max.x + 5.0f;
    _boundingBox.max.x += 100.0f;

    _inputBox = _boundingBox;
}

void EnumSelectable::nextEnum( void)
{
    _activeEnum++;
    if( _activeEnum == _enumList.end())
    {
        _activeEnum = _enumList.begin();
    }
        
    Value *v = new Value( *_activeEnum);
    ConfigS::instance()->updateKeyword( _variable, v);    
    AudioS::instance()->playSample( "sounds/click1.wav");
}

void EnumSelectable::prevEnum( void)
{
    if( _activeEnum == _enumList.begin())
    {
        _activeEnum = _enumList.end();
    }
    _activeEnum--;

    Value *v = new Value( *_activeEnum);
    ConfigS::instance()->updateKeyword( _variable, v);    
    AudioS::instance()->playSample( "sounds/click1.wav");
}

void EnumSelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &/*offset*/)
{
    if( !_enabled) return;
    
    if( !isDown) return;

    switch( trigger.type)
    {
        case eKeyTrigger:
            switch( trigger.data1)
            {
                case SDLK_LEFT:
                    prevEnum();
                    break;
                    
                case SDLK_RIGHT:
                    nextEnum();
                    break;
                    
                default:
                    break;
            }
            break;
            
	case eButtonTrigger:
	    {
                if( trigger.data1 == SDL_BUTTON_WHEELDOWN)
                {
                    prevEnum();
                }
                else
                {
                    nextEnum();
                }
	    }
	    break;

	case eMotionTrigger:
	    this->activate();
	    break;

	default:
	    break;
    }
}

void EnumSelectable::activate( bool beQuiet)
{
    updateActive( beQuiet ? eNoFeedback : eBeep);
}

void EnumSelectable::draw( const Point2Di &offset)
{
    TextOnlySelectable::draw(offset);

    string val = *_activeEnum;

    glColor4f(1.0, 1.0, 1.0, 1.0);
    _fontShadow->DrawString( 
	val.c_str(), 
	_xOff + offset.x+9*_size,
        _boundingBox.min.y + offset.y-9*_size, 
	_size, _size);
    
    if( _active == this)
    {
        if( !_enabled)
            glColor4f(0.7, 0.7, 0.7, 1.0);
        else
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        if( !_enabled)
            glColor4f(0.5, 0.5, 0.5, 1.0);
        else
            glColor4f(1.0f, 0.852f, 0.0f, 1.0f);        
    }
        
    _fontWhite->DrawString( 
	val.c_str(), 
	_xOff + offset.x,
        _boundingBox.min.y + offset.y, 
	_size, _size); 
}

//------------------------------------------------------------------------------

BoolSelectable::BoolSelectable( 
    bool enabled,
    const BoundingBox &rect, 
    const string &text,
    const string &info, 
    const string &variable): 

    TextOnlySelectable(enabled, rect, text, info, false, 0.7f, 1.0f, 1.0f, 1.0f),
    _variable(variable)
{
    _checkmark = _icons->getIndex( "Checkmark");
    _checkmarkOff = _icons->getIndex( "CheckmarkOff");

    _xOff = _boundingBox.max.x + 10.0f;
    _boundingBox.max.x = _xOff + _icons->getWidth( _checkmark)*0.5f;

    _inputBox = _boundingBox;
}

void BoolSelectable::select( void)
{
    if( !_enabled) return;

    toggle();
}

void BoolSelectable::toggle( void)
{
    bool val = false;
    ConfigS::instance()->getBoolean( _variable, val);
    Value *v = new Value( !val);
    
//    LOG_INFO << "New value for " << _variable 
//             << " is " << v->getString() << endl;
    
    ConfigS::instance()->updateKeyword( _variable, v);
    AudioS::instance()->playSample( "sounds/click1.wav");
}

void BoolSelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &/*offset*/)
{
    if( !_enabled) return;
    
    if( !isDown) return;

    switch( trigger.type)
    {
	case eButtonTrigger:
	    {
                toggle();
	    }
	    break;

	case eMotionTrigger:
	    this->activate();
	    break;

	default:
	    break;
    }
}

void BoolSelectable::activate( bool beQuiet)
{
    updateActive( beQuiet ? eNoFeedback : eBeep);
}

void BoolSelectable::draw( const Point2Di &offset)
{
    TextOnlySelectable::draw(offset);

    bool val = false;
    ConfigS::instance()->getBoolean( _variable, val);

    _icons->bind();
    if( _active == this)
    {
        if( !_enabled)
            glColor4f(0.7, 0.7, 0.7, 1.0);
        else
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        if( !_enabled)
            glColor4f(0.5, 0.5, 0.5, 1.0);
        else
            glColor4f(1.0, 1.0, 1.0, 1.0);
    }
        
    glEnable(GL_TEXTURE_2D);
    if( val)
    {
	_icons->Draw( _checkmark, 
                      _xOff + offset.x, 
                      _boundingBox.min.y + offset.y+2, 0.5, 0.5);
    }
    else
    {
	_icons->Draw( _checkmarkOff, 
                      _xOff + offset.x, 
                      _boundingBox.min.y + offset.y+2, 0.5, 0.5);
    }
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

LeaderBoardSelectable::LeaderBoardSelectable( 
    bool enabled,
    const BoundingBox &r, 
    const string &text,
    const string &info): 

    Selectable(enabled, r, info),
    _text(text),
    _size(1.0f)
{
    _fontShadow = FontManagerS::instance()->getFont( "bitmaps/menuShadow");

    float width  = _fontWhite->GetWidth( _text.c_str(), _size);
    float height = _fontWhite->GetHeight( _size);

    _boundingBox.min.x -= width*0.5f;  
    _boundingBox.max.x = _boundingBox.min.x + width;
    _boundingBox.max.y = _boundingBox.min.y + height;

    _inputBox.min.x =   0;
    _inputBox.min.y =  30;
    _inputBox.max.x = 600;
    _inputBox.max.y = 470;
    
    //ScoreKeeperS::instance()->setActive("");
}

LeaderBoardSelectable::~LeaderBoardSelectable()
{
}

void LeaderBoardSelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &offset)
{
    if( !_enabled) return;
    
    if( !isDown) return;

    float mouseY = trigger.fData2;

    switch( trigger.type)
    {
	case eButtonTrigger:
	    break;

	case eMotionTrigger:
	    {
                float spacing = 36.0f;

		int idx = (int)((mouseY+offset.y-100.0f)/spacing);
		if( idx < 0) idx = 0;
		if( idx > 9) idx = 9;
		_info = ScoreKeeperS::instance()->getInfoText( 9-idx);
//		LOG_INFO << "spot = " << idx << endl;
	    }
	    this->activate();
	    break;

	default:
	    break;
    }
}

void LeaderBoardSelectable::activate( bool /*beQuiet*/)
{
    updateActive( eNoFeedback);
}

void LeaderBoardSelectable::draw( const Point2Di &offset)
{
#if 0
    glColor4f( 0.2, 0.2, 1.0, 0.5);
    vec3f v[4] = {
        vec3f( _inputBox.min.x+offset.x, _inputBox.min.y+offset.y, 0 ),
        vec3f( _inputBox.min.x+offset.x, _inputBox.max.y+offset.y, 0 ),
        vec3f( _inputBox.max.x+offset.x, _inputBox.max.y+offset.y, 0 ),
        vec3f( _inputBox.max.x+offset.x, _inputBox.min.y+offset.y, 0 ),
    };
    GLVBO::DrawQuad( GL_QUADS, v);    
#endif

    Selectable::draw(offset);
#if 0
    //Draw "Leader Board:" sub-header
    glColor4f(1.0, 1.0, 1.0, 1.0);
    _fontShadow->DrawString( 
	_text.c_str(), 
	_boundingBox.min.x + offset.x+9*_size,
        _boundingBox.min.y + offset.y-9*_size, 
	_size, _size); 

    glColor4f(1.0, 1.0, 1.0, 1.0);
    _fontWhite->DrawString( 
	_text.c_str(), 
	_boundingBox.min.x + offset.x,
        _boundingBox.min.y + offset.y, 
	_size, _size); 
#endif
    ScoreKeeperS::instance()->draw(offset);
}


//------------------------------------------------------------------------------


bool ResolutionSelectable::DescendingAreaOrder( const Resolution &res1, 
                                                const Resolution &res2)
{
    return res1.width*res1.height < res2.width*res2.height;
}

ResolutionSelectable::ResolutionSelectable(
    bool enabled,
    const BoundingBox &rect, 
    const string &text,
    const string &info): 

    TextOnlySelectable(enabled, rect, text, info, false, 0.7f, 1.0f, 1.0f, 1.0f)
{
    _fontShadow = FontManagerS::instance()->getFont( "bitmaps/menuShadow");

    _checkmark = _icons->getIndex( "Checkmark");
    _checkmarkOff = _icons->getIndex( "CheckmarkOff");

    //Get current resolution
    int currentWidth = 640;
    ConfigS::instance()->getInteger( "width", currentWidth);
    int currentHeight = 480;
    ConfigS::instance()->getInteger( "height", currentHeight);
    
    //get custom resolutions from config file
    string resolutions;
    if( !ConfigS::instance()->getString( "resolutions", resolutions))
    {
	resolutions = "512x384,640x480,800x600,1024x768,1152x864,1280x960,1600x1200";
	Value *res = new Value( resolutions);
	ConfigS::instance()->updateKeyword( "resolutions", res);
    }

    //extract resolutions
    Tokenizer rToken(resolutions, ",");
    int count = 1;
    while( count < 20)
    {
	string nextRes = rToken.next();
	if( nextRes == "") break;

	Tokenizer t(nextRes, "x");

	int width = atoi(t.next().c_str());
	int height = atoi(t.next().c_str());

	_resolutionList.push_back( Resolution(width, height));

	count++;
    }

    //Add other fullscreen resolutions
    addFullscreenResolutions();
    
    if( _resolutionList.size() == 0 )
    {
        _resolutionList.push_back( Resolution(currentWidth, currentHeight));        
    }
    
    //sort resolutions by area
    std::sort(_resolutionList.begin(), _resolutionList.end(), DescendingAreaOrder);

    float fwidth  = 0;
    float fheight = _fontWhite->GetHeight( _size);

    _activeResolution = _resolutionList.begin();
    vector<Resolution>::iterator i;
    for( i=_resolutionList.begin(); i!=_resolutionList.end(); i++)
    {
        Resolution res = (*i);
	if( ((*i).width == currentWidth) && ((*i).height == currentHeight))
	{
	    _activeResolution = i;
	}
        
        float nextfwidth  = _fontWhite->GetWidth( (*i).text.c_str(), _size);
        if( nextfwidth > fwidth) fwidth = nextfwidth;
    }
    

//    _boundingBox.min.x -= fwidth*0.5;  
    _boundingBox.max.x = 
        _boundingBox.min.x + fwidth + 
            _fontWhite->GetWidth( _text.c_str(), _size) +
        _icons->getWidth( _checkmark)*0.5f + 10;
    _boundingBox.max.y = _boundingBox.min.y + fheight;

    //bounding box for checkmark 
    _bRect.min.x = _boundingBox.max.x - _icons->getWidth( _checkmark)*0.5f;
    _bRect.max.x = _boundingBox.max.x;
    _bRect.min.y = _boundingBox.min.y ;
    _bRect.max.y = _bRect.min.y + 30;

    _inputBox = _boundingBox;
}

void ResolutionSelectable::select( void)
{
    applyResolution();
}

void ResolutionSelectable::applyResolution( void)
{
    int width = (*_activeResolution).width;
    int height = (*_activeResolution).height;
    
    Value *w = new Value( width);
    ConfigS::instance()->updateKeyword( "width", w);
    Value *h = new Value( height);
    ConfigS::instance()->updateKeyword( "height", h);
    AudioS::instance()->playSample( "sounds/click1.wav");
}

void ResolutionSelectable::addFullscreenResolutions(void)
{
    SDL_Rect **modes=SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_OPENGL);

    if( modes == (SDL_Rect **)0)
    {
	return;
    }

    if( modes == (SDL_Rect **)-1)
    {
	return;
    }

    for(int i=0; modes[i]; i++)
    {
	Resolution newRes( modes[i]->w, modes[i]->h);
        
	bool add = true;
	vector<Resolution>::iterator res;
        for( res=_resolutionList.begin(); res!=_resolutionList.end(); res++)
        {
	    if( (*res) == newRes) 
	    {
		//LOG_INFO << "Found dup " << buf << endl;
		add = false;
		break;
	    }            
        }

	if( add)
	{
	    //LOG_INFO << "Adding new " << newRes.text << endl;
	    _resolutionList.push_back( newRes);
	}
    }
}

void ResolutionSelectable::prevResolution( void)
{
    if( _activeResolution == _resolutionList.begin())
    {
        _activeResolution = _resolutionList.end();
    }
    _activeResolution--;
    AudioS::instance()->playSample( "sounds/tick1.wav");
}

void ResolutionSelectable::nextResolution( void)
{
    _activeResolution++;
    if( _activeResolution == _resolutionList.end())
    {
        _activeResolution = _resolutionList.begin();
    }
    AudioS::instance()->playSample( "sounds/tick1.wav");
}

void ResolutionSelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &offset)
{
    if( !_enabled) return;
    
    if( !isDown) return;

    float mouseX = trigger.fData1;
    float mouseY = trigger.fData2;

    switch( trigger.type)
    {
        case eKeyTrigger:
            switch( trigger.data1)
            {
                case SDLK_LEFT:
                    prevResolution();
                    break;
                    
                case SDLK_RIGHT:
                    nextResolution();
                    break;
                    
                default:
                    break;
            }
            break;
            
	case eButtonTrigger:
	    {
		if( isDown)
		{
		    if( (mouseX >= (_bRect.min.x+offset.x)) &&
			(mouseX <= (_bRect.max.x+offset.x)) &&
			(mouseY >= (_bRect.min.y+offset.y)) &&
			(mouseY <= (_bRect.max.y+offset.y)) )
		    {
                        applyResolution();
		    }
		    else
		    {
                        if( trigger.data1 == SDL_BUTTON_WHEELDOWN)
                        {
                            prevResolution();
                        }
                        else
                        {
                            nextResolution();
                        }
		    }
		}
	    }
	    break;

	case eMotionTrigger:
	    this->activate();
	    break;

	default:
	    break;
    }
}

void ResolutionSelectable::activate( bool beQuiet)
{
    updateActive( beQuiet ? eNoFeedback : eBeep);
}

void ResolutionSelectable::draw( const Point2Di &offset)
{
//    Selectable::draw(offset);
    TextOnlySelectable::draw(offset);
    string resolution = /*_text +*/ (*_activeResolution).text;

    float xOff = _fontWhite->GetWidth( _text.c_str(), _size);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    _fontShadow->DrawString( 
	resolution.c_str(), 
	xOff+_boundingBox.min.x + offset.x+9*_size,
        _boundingBox.min.y + offset.y-9*_size, 
	_size, _size); 

    glColor4f(1.0f, 0.852f, 0.0f, 1.0f);
//    glColor4f(1.0, 1.0, 1.0, 1.0);
    _fontWhite->DrawString( 
	resolution.c_str(), 
	xOff+_boundingBox.min.x + offset.x, 
        _boundingBox.min.y + offset.y, 
	_size, _size); 

    _icons->bind();
    if( _active == this)
    {
        if( !_enabled)
            glColor4f(0.7, 0.7, 0.7, 1.0);
        else
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        if( !_enabled)
            glColor4f(0.5, 0.5, 0.5, 1.0);
        else
            glColor4f(1.0, 1.0, 1.0, 1.0);
    }
    
    glEnable(GL_TEXTURE_2D);
    float _xOff = _bRect.min.x;

    int currentWidth;
    ConfigS::instance()->getInteger( "width", currentWidth);
    int currentHeight;
    ConfigS::instance()->getInteger( "height", currentHeight);

    int width = (*_activeResolution).width;
    int height = (*_activeResolution).height;

    if( (currentWidth == width) && (currentHeight == height))
    {
	_icons->Draw( _checkmark, 
                      _xOff + offset.x, 
                      _boundingBox.min.y + offset.y+2, 0.5, 0.5);
    }
    else
    {
	_icons->Draw( _checkmarkOff, 
                      _xOff + offset.x, 
                      _boundingBox.min.y + offset.y+2, 0.5, 0.5);
    }
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

TextSelectable::TextSelectable( 
    bool enabled, 
    const BoundingBox &rect, 
    const string &text,
    const string &info): 

    TextOnlySelectable(enabled, rect, text, info),
    _ds(0.0)
{
    _prevSize = _size;
    _minSize = 1.0;
    _maxSize = 2.0;
}

void TextSelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &/*offset*/)
{
    if( !_enabled) return;

    if( !isDown) return;

    switch( trigger.type)
    {
	case eButtonTrigger:
	    MenuManagerS::instance()->Enter();
	    break;

	case eMotionTrigger:
	    this->activate();
	    break;

	default:
	    break;
    }
}

void TextSelectable::activate( bool beQuiet)
{
    updateActive( beQuiet ? eNoFeedback : eBeep);
    _ds = 0.1f;        
}

void TextSelectable::deactivate( void)
{
//	LOG_INFO << "Deactivate " << _text << endl;
    _ds = -0.1f;
}

void TextSelectable::update( void)
{
    if( !_enabled) return;
    
    _prevSize = _size;
    _size += _ds;
    Clamp( _size, _minSize, _maxSize-0.2); //any bigger and we'll have overlapping activation areas

    //adjust the input box according to the scaled text
    float dx = (float)(_boundingBox.max.x - _boundingBox.min.x) * (_size-1.0f) / 2.0f;
    float dy = (float)(_boundingBox.max.y - _boundingBox.min.y) * (_size-1.0f) / 2.0f;

    _inputBox.min.x = _boundingBox.min.x - dx;
    _inputBox.min.y = _boundingBox.min.y - dy;
    _inputBox.max.x = _boundingBox.max.x + dx;
    _inputBox.max.y = _boundingBox.max.y + dy;
}

void TextSelectable::draw( const Point2Di &offset)
{
    Selectable::draw(offset);

    float iSize = _prevSize + (_size - _prevSize) * 
	GameState::frameFractionOther;
    Clamp( iSize, _minSize, _maxSize);

    float halfWidth = _fontWhite->GetWidth( _text.c_str(), iSize-1.0f)/2.0f;
    float halfHeight = _fontWhite->GetHeight( iSize-1.0f)/2.0f;

    glColor4f(1.0, 1.0, 1.0, 1.0);
    _fontShadow->DrawString( 
	_text.c_str(), 
	_boundingBox.min.x + offset.x-halfWidth+5*iSize,
	_boundingBox.min.y + offset.y-halfHeight-5*iSize, 
	iSize, iSize);

    if( !_enabled)
        glColor4f(0.5, 0.5, 0.5, 1.0);
    else
        glColor4f(r, g, b, 0.8f);
    _fontWhite->DrawString( 
	_text.c_str(), 
	_boundingBox.min.x + offset.x-halfWidth, 
        _boundingBox.min.y + offset.y-halfHeight, 
	iSize, iSize); 
}

//------------------------------------------------------------------------------

ActionSelectable::ActionSelectable(
    bool enabled,
    const BoundingBox &rect, 
    const string &action,
    const string &text,
    const string &info): 

    TextSelectable(enabled, rect, text, info),
    _action(action)
{
}

void ActionSelectable::select( void)
{
//	LOG_INFO << "Selecting: " << _action << endl;
    if( _action == "NewGame")
    {
	GameS::instance()->startNewGame();

	//no confirm sound in this case
	return;
    }
    else if( _action == "Quit")
    {
	GameState::isAlive = false;
    }
    AudioS::instance()->playSample( "sounds/confirm.wav");
}

//------------------------------------------------------------------------------

MenuSelectable::MenuSelectable(
	TiXmlNode *node,
        bool enabled,
	const BoundingBox &rect,
	const string &text,
	const string &info):

    TextSelectable(enabled, rect, text, info),
    _node(node)
{
}

void MenuSelectable::select( void)
{
    MenuManagerS::instance()->makeMenu( _node);
    AudioS::instance()->playSample( "sounds/confirm.wav");
}


//------------------------------------------------------------------------------

BindKeySelectable::BindKeySelectable( bool enabled, 
                                      const BoundingBox &rect,
                                      const string &text,
                                      const string &info, 
                                      const std::string &action, 
                                      float size):
TextOnlySelectable(enabled, rect, text, info, false, size, 1.0f, 1.0f, 1.0f),
_bindKeyOn(0),
_bindKeyOff(0),
_action(action)
{
    _icons = BitmapManagerS::instance()->getBitmap( "bitmaps/scoreBoard");

    _bindKeyOn = _icons->getIndex( _action+"On");
    if( _bindKeyOn == -1)
    {
	LOG_ERROR << "Icon-On for action not found" << endl;
    }
    
    _bindKeyOff = _icons->getIndex( _action+"Off");
    if( _bindKeyOff == -1)
    {
	LOG_ERROR << "Icon-Off for action not found" << endl;
    }

    _size = _size * 0.6;
    _boundingBox.max.x = _boundingBox.min.x + _icons->getWidth( _bindKeyOn)*_size;
    _boundingBox.max.y = _boundingBox.min.y + _icons->getHeight( _bindKeyOn)*_size;
    _inputBox = _boundingBox;
}

void BindKeySelectable::select( void)
{
    InputS::instance()->bindNextTrigger( _action);
}

void BindKeySelectable::input( const Trigger &trigger, const bool &isDown, const Point2Di &/*offset*/)
{
    if( !_enabled) return;

    if( !isDown) return;
    
    switch( trigger.type)
    {
	case eButtonTrigger:
            InputS::instance()->bindNextTrigger( _action);
	    break;
            
	case eMotionTrigger:
	    this->activate();
	    break;
            
	default:
	    break;
    }
}

void BindKeySelectable::activate( bool beQuiet)
{
    updateActive( beQuiet ? eNoFeedback : eBeep);
}

void BindKeySelectable::draw( const Point2Di &offset)
{
    _text = InputS::instance()->getTriggerName(_action);
    //TextOnlySelectable::draw(offset);
    
    Selectable::draw(offset);
    
    float fontScale = _size * 1.8;
    float center = (_boundingBox.max.x - _boundingBox.min.x - _fontWhite->GetWidth(_text.c_str(), fontScale)) / 2.0 ;
    glColor4f(1.0, 1.0, 1.0, 1.0);
    _fontShadow->DrawString( 
                             _text.c_str(), 
                             center + _boundingBox.min.x + offset.x +9*fontScale,
                             _boundingBox.min.y + offset.y -9*fontScale - 20.0, 
                             fontScale, fontScale); 
    
    if( !_enabled)
        glColor4f(0.5, 0.5, 0.5, 1.0);
    else    
        glColor4f(r, g, b, 1.0);
    _fontWhite->DrawString( 
                            _text.c_str(), 
                            center + _boundingBox.min.x + offset.x, 
                            _boundingBox.min.y + offset.y - 20.0, 
                            fontScale, fontScale); 
    
    
    _icons->bind();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    if( _active == this)
    {
        if( InputS::instance()->waitingForBind())
        {
            glColor4f(1.0f, 0.852f, 0.0f, 1.0f);            
        }
        else
        {            
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        }
	_icons->Draw( _bindKeyOn, 
                      _boundingBox.min.x + offset.x, 
                      _boundingBox.min.y + offset.y, _size, _size);
    }
    else
    {
	_icons->Draw( _bindKeyOff, 
                      _boundingBox.min.x + offset.x, 
                      _boundingBox.min.y + offset.y, _size, _size);
    }
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------
