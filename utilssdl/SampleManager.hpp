#pragma once
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
#include <string>

#include "ResourceCache.hpp"
#include "SDL2/SDL_mixer.h"

class SampleManager : public ResourceCache<Mix_Chunk> {
public:
    SampleManager(void);
    virtual ~SampleManager();

    Mix_Chunk* getSample(const std::string& sampleName) { return getResource(sampleName); }

protected:
    virtual Mix_Chunk* load(const std::string& wav);

private:
    SampleManager(const SampleManager&);
    SampleManager& operator=(const SampleManager&);
};
