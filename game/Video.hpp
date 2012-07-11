#pragma once
// Description:
//   Video subsystem.
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
#include <string>

#include <GLTexture.hpp>
#include <GLBitmapFont.hpp>
#include <TextInput.hpp>

#include <Singleton.hpp>

const int VIDEO_DEFAULT_WIDTH=640;
const int VIDEO_DEFAULT_HEIGHT=480;

const int VIDEO_ORTHO_WIDTH=1000;
const int VIDEO_ORTHO_HEIGHT=750;

class Video
{
friend class Singleton<Video>;
public:
    bool init( void);
    bool update( void);
    void updateLogic( void);

    int getWidth( void) { return _width;}
    int getHeight( void) { return _height;}

    void toggleCritterBoard( void)
    {
	_boardVisible = !_boardVisible;
    }

    void takeSnapshot( void);

private:
    ~Video();
    Video( void);
    Video( const Video&);
    Video &operator=(const Video&);

    void reset( void);
    void reload( void);
    bool updateSettings( void);
    bool setVideoMode( void);

    bool _isFullscreen;
    bool _showStarfield;
    bool _showNebulas;

    int _maxFPS;
    float _fpsStepSize;

    int _bpp;
    int _width;
    int _height;

    GLBitmapFont *_smallFont;
    GLBitmapFont *_scoreFont;
    GLBitmapFont *_gameOFont;

    GLBitmapCollection *_board;
    int _boardIndex;
    bool _boardVisible;
    float _boardPosX;

    std::string _statusMsg;
    bool _statusMsgActive;
    float _statusMsgWidth;
    float _statusMsgPos;

    GLTexture *_titleA;
    GLTexture *_titleB;

    float _angle;
    float _prevAngle;

    TextInput _textInput;
};

typedef Singleton<Video> VideoS;
