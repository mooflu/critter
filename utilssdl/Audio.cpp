// Description:
//   Audio subsysytem.
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
#include "Audio.hpp"

#include "Trace.hpp"
#include "Config.hpp"
#include "Value.hpp"
#include "ResourceManager.hpp"
#ifdef IPHONE
#else
#include "zrwops.hpp"
#endif
#include "SampleManager.hpp"
#include "Timer.hpp"
#include "GetDataPath.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

Audio::Audio():
  _sampleManager(0),
  _soundTrack(0),
  _soundTrackData(0),
  _defaultSoundtrack(""),
  _playDefaultSoundtrack( true),
  _playMusic( true),
  _isPlaying( false),
  _unloadMusic(false),
  _musicVolume(0.8),
  _effectsVolume(0.8),
  _audioEnabled(true)
{
    XTRACE();

    bool dummy;
    //if config variables don't exist (upgrade, etc) update 
    if( ! ConfigS::instance()->getBoolean( "playMusic", dummy))
    {
	Value *v = new Value( _playMusic);
	ConfigS::instance()->updateKeyword( "playMusic", v);
    }
    if( ! ConfigS::instance()->getBoolean( 
	"playDefaultSoundtrack", dummy))
    {
	Value *v = new Value( _playDefaultSoundtrack);
	ConfigS::instance()->updateKeyword( "playDefaultSoundtrack", v);
    }
}

Audio::~Audio()
{
    XTRACE();
    LOG_INFO << "Audio shutdown...\n";

    turnMusicOff();
    unloadMusic();

    Mix_CloseAudio();
    delete _sampleManager;

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

static void musicFinished( void)
{
    AudioS::instance()->musicFinished();
}

void Audio::musicFinished( void)
{
    _unloadMusic = true;
}

void Audio::loadMusic( const string &mod)
{
    XTRACE();
#ifndef IPHONE
    if( mod == "") return;

    LOG_INFO << "Loading soundtrack...[" << mod << "]\n";
    unloadMusic();

    if( !ResourceManagerS::instance()->selectResource( mod))
    {
	LOG_ERROR << "Music file [" << mod << "] not found.\n";
	return;
    }

    ziStream &infile = ResourceManagerS::instance()->getInputStream();

    int fileSize = infile.fileSize();
    _soundTrackData = new char[ fileSize];
    infile.read( _soundTrackData, fileSize);
    SDL_RWops *src = SDL_RWFromMem( _soundTrackData, fileSize); 

    _soundTrack = Mix_LoadMUS_RW(src);

    if( !_soundTrack)
    { 
	LOG_ERROR << "Failed to load soundtrack: [" << mod << "]\n";
	LOG_ERROR << SDL_GetError() << "\n";
    }
    else
    {
	//loop soundtrack
	Mix_FadeInMusic( _soundTrack, -1, 500);
	Mix_HookMusicFinished(::musicFinished);
	_isPlaying = true;
    }
#endif
}

bool Audio::init( void)
{
    XTRACE();
    LOG_INFO << "Initializing Audio...\n";

    bool audio;
    if( !ConfigS::instance()->getBoolean( "audio", audio))
    {
        audio = false;
    }
    if( audio)
    {
	if( SDL_InitSubSystem( SDL_INIT_AUDIO) < 0 )
	{
	    LOG_ERROR << "Init Audio: failed # " << SDL_GetError() << "\n";
	    return false;
	}

        if( Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) < 0 ) 
        {
	    LOG_ERROR << "Init Audio (Mixer): failed # " 
            << SDL_GetError() << "\n";
	    return false;
        }

	_sampleManager = new SampleManager();

	//make sure we have default volumes
	float dummy;
	if( ! ConfigS::instance()->getFloat( "musicVolume", dummy))
	{
	    Value *v = new Value( 0.8f);
	    ConfigS::instance()->updateKeyword( "musicVolume", v);
	}
	if( !ConfigS::instance()->getFloat( "effectsVolume", dummy))
	{
	    Value *v = new Value( 0.8f);
	    ConfigS::instance()->updateKeyword( "effectsVolume", v);
	}

	ConfigS::instance()->getBoolean( "playMusic", _playMusic);
	ConfigS::instance()->getBoolean( 
	    "playDefaultSoundtrack", _playDefaultSoundtrack);
	startMusic();
	updateVolume();

        LOG_INFO << "Audio ON.\n";
    }
    else
    {
        LOG_INFO << "Audio OFF.\n";
    }

    return true;
}

void Audio::playSample( const string &sampleName)
{
    if( !_sampleManager || !_audioEnabled) return;

#ifdef IPHONE
    string fullName = sampleName + ".caf";
    Mix_Chunk *sample = _sampleManager->getSample( fullName);
#else
    string fullName = sampleName;
    Mix_Chunk *sample = _sampleManager->getSample( fullName);
    if( !sample)
    {
        fullName = sampleName + ".wav";
        sample = _sampleManager->getSample( fullName);        
    }
#endif
    if( sample)
    {
	//any channel, no loop
	Mix_PlayChannel(-1, sample, 0);
    }
}

void Audio::turnMusicOff( void)
{
    if( _isPlaying)
    {
	Mix_FadeOutMusic(500);
	_isPlaying = false;
    }
}

void Audio::setDefaultSoundtrack( const string &fileName)
{
    _defaultSoundtrack = fileName;
}

void Audio::startMusic( void)
{
    if( _playMusic)
    {
	string soundtrack = "";

	if( _playDefaultSoundtrack)
	{
	    soundtrack = _defaultSoundtrack;
	}
	else
	{
	    ConfigS::instance()->getString( "soundtrack", soundtrack);
	}

        turnMusicOff();
	loadMusic( soundtrack);
    }
    else
    {
        turnMusicOff();
    }
}

void Audio::updateSettings( void)
{
    if( Mix_FadingMusic() != MIX_NO_FADING) return;

    bool oldPlayMusic = _playMusic;
    bool oldPlayDefaultSoundtrack = _playDefaultSoundtrack;

    ConfigS::instance()->getBoolean( "playMusic", _playMusic);
    ConfigS::instance()->getBoolean( 
	"playDefaultSoundtrack", _playDefaultSoundtrack);

    if( (oldPlayMusic == _playMusic) &&
	(oldPlayDefaultSoundtrack == _playDefaultSoundtrack))
    {
	//no changes...
	return;
    }

    startMusic();
}

void Audio::updateVolume( void)
{
    int newVolume;

    float musicVolume = 0.8;
    ConfigS::instance()->getFloat( "musicVolume", musicVolume);
    newVolume = (int)(MIX_MAX_VOLUME * musicVolume);
    if( _musicVolume != musicVolume)
    {
	_musicVolume = musicVolume;
	Mix_VolumeMusic( newVolume);
    }

    float effectsVolume = 0.8;
    ConfigS::instance()->getFloat( "effectsVolume", effectsVolume);
    newVolume = (int)(MIX_MAX_VOLUME * effectsVolume);
    if( _effectsVolume != effectsVolume)
    {
	_effectsVolume = effectsVolume;
	Mix_Volume( -1, newVolume);
	//playSample( "sounds/beep");
    }
}

void Audio::unloadMusic( void)
{
    if( _soundTrack)
    {
	Mix_FreeMusic( _soundTrack);
	_soundTrack = 0;
	delete [] _soundTrackData;
	_soundTrackData = 0;
    }
}

bool Audio::update( void)
{
    if( !_sampleManager || ! _audioEnabled) return true;

    if( _unloadMusic)
    {
	unloadMusic();
	_unloadMusic = false;
    }

    updateVolume();

    static float nextTime = Timer::getTime()+0.5f;
    float thisTime = Timer::getTime();
    if( thisTime > nextTime)
    {
	updateSettings();
	nextTime = thisTime+0.5f;
    }

    return true;
}
