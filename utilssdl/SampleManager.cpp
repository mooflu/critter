// Description:
//   Sample Manager.
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
#include "SampleManager.hpp"

#include "Trace.hpp"
#include "ResourceManager.hpp"
#include "zrwops.hpp"

#include "SDL2/SDL_mixer.h"

#include <memory>
using namespace std;

SampleManager::SampleManager(void) {
    XTRACE();
}

SampleManager::~SampleManager() {
    //need to override base destructor behaviour, since we need
    //to Mix_FreeChunk not delete.

    hash_map<const string, Mix_Chunk*, hash<const string>>::const_iterator ci;
    for (ci = _resourceMap.begin(); ci != _resourceMap.end(); ci++) {
        Mix_Chunk* res = ci->second;
        Mix_FreeChunk(res);
    }
    _resourceMap.clear();
}

Mix_Chunk* SampleManager::load(const string& wav) {
    XTRACE();
#ifdef IPHONE
    Mix_Chunk* mix = Mix_Load(wav);
#else
    string theWav = wav;
    if (!ResourceManagerS::instance()->hasResource(theWav)) {
        theWav += ".wav";
        if (!ResourceManagerS::instance()->hasResource(theWav)) {
            LOG_ERROR << "Wav file [" << wav << "] not found." << endl;
            return 0;
        }
    }

    std::shared_ptr<ziStream> infilePtr(ResourceManagerS::instance()->getInputStream(theWav));
    ziStream& infile = *infilePtr;
    SDL_RWops* src = RWops_from_ziStream(infile);

    Mix_Chunk* mix = Mix_LoadWAV_RW(src, 0);
    if (!mix) {
        LOG_ERROR << "Failed to load wav: [" << wav << "]" << endl;
        LOG_ERROR << SDL_GetError() << endl;
        return 0;
    }
    SDL_RWclose(src);
#endif
    return mix;
}
