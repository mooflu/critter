#pragma once
// Description:
//   Stage manager controls movement from levelpack to levelpack and
//   level to level.
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

#include <Singleton.hpp>
#include <GameState.hpp>
#include <LevelPack.hpp>

#include <tinyxml.h>

#include <string>
#include <vector>

class StageManager {
    friend class Singleton<StageManager>;

public:
    bool init(void);
    void reset(void);
    void update(void);

    float levelStartTime(void) { return _levelStartTime; }

    const std::string& getActiveLevelName(void) { return _activeLevelName; }

private:
    StageManager(void) :
        _activeLevelPack(0),
        _activeLevel(0),
        _delayEndOfLevel(30) {}

    ~StageManager() {
        delete _activeLevelPack;
        _activeLevelPack = 0;
        _activeLevel = 0;
    }

    StageManager(const StageManager&);
    StageManager& operator=(const StageManager&);

    TiXmlDocument* _activeLevelPack;
    TiXmlNode* _activeLevel;
    int _delayEndOfLevel;
    float _levelStartTime;
    std::string _activeLevelName;

    list<std::string> _levelPackList;
    list<std::string>::iterator _levelPackIterator;
    std::vector<TiXmlNode*> _levelList;
    unsigned int _activeLevelIndex;

    bool findLevelPacks(void);
    bool loadNextLevelPack(void);
    bool activateLevel(void);
};

typedef Singleton<StageManager> StageManagerS;
