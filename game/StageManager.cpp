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
#include "Trace.hpp"
#include "Constants.hpp"
#include "GameState.hpp"
#include "StageManager.hpp"
#include "EnemyFactory.hpp"
#include "Skill.hpp"
#include "XMLHelper.hpp"
#include "Config.hpp"
#include "Hero.hpp"

#include "RandomKnuth.hpp"
#include "ParticleInfo.hpp"
#include "Particles.hpp"
#include "ParticleGroup.hpp"
#include "ParticleGroupManager.hpp"

static RandomKnuth _random;

bool StageManager::init(void) {
    return findLevelPacks();
}

void StageManager::reset(void) {
    //catch used skill changes via runtime config
    SkillS::instance()->updateSkill();

    _levelPackIterator = _levelPackList.begin();
    loadNextLevelPack();
    activateLevel();
}

void StageManager::update(void) {
    static ParticleGroup* bonus = ParticleGroupManagerS::instance()->getParticleGroup(BONUS_GROUP);

    //no armor piercing needed for rookie
    if ((GameState::skill != Skill::eRookie) && (HeroS::instance()->getArmorPierce() <= 1.0f)) {
        if (_random.rangef0_1() < 0.001f) {
            LOG_INFO << "ArmorPierce" << endl;
            float posX = (_random.rangef0_1() - 0.5f) * 60.0f;
            bonus->newParticle("ArmorPierce", posX, 49.0f, -100.0f);
        }
    }

    if (GameState::numObjects == 0) {
        if (_delayEndOfLevel-- > 0) {
            return;
        }

        _activeLevelIndex++;
        if (_activeLevelIndex >= _levelList.size()) {
            loadNextLevelPack();
        } else {
            _activeLevel = _levelList[_activeLevelIndex];
        }
        activateLevel();
    }
}

bool StageManager::findLevelPacks(void) {
    list<string> rList;
    ResourceManagerS::instance()->getFiles("levelpacks", rList);

    list<string>::iterator i;
    for (i = rList.begin(); i != rList.end(); i++) {
        string resourceName = "levelpacks/"  + (*i);

        string::size_type end = resourceName.length() - 8;  // strlen("Pack.xml")
        string::size_type find = resourceName.find("Pack.xml");
        if ((find != string::npos) && (find == end)) {
            LOG_INFO << "Adding LevelPack [" << resourceName << "]" << endl;
            _levelPackList.insert(_levelPackList.end(), resourceName);
        }
    }

    _levelPackIterator = _levelPackList.begin();

    bool foundSome = _levelPackIterator != _levelPackList.end();
    if (!foundSome) {
        LOG_WARNING << "No levelpacks found in resource file!" << endl;
        string defaultLevelPack = "levelpacks/CritterPack.xml";
        _levelPackList.insert(_levelPackList.end(), defaultLevelPack);
    }

    return true;
}

bool StageManager::loadNextLevelPack(void) {
    XTRACE();

    if (_levelPackIterator == _levelPackList.end()) {
        _levelPackIterator = _levelPackList.begin();
        //when we wrap around, increment skill
        SkillS::instance()->incrementSkill();
    }

    delete _activeLevelPack;

    string levelPackName = *_levelPackIterator;
    _activeLevelPack = XMLHelper::load(levelPackName);

    if (!_activeLevelPack) {
        _activeLevel = 0;
        return false;
    }

    TiXmlNode* levelPack = _activeLevelPack->FirstChild("LevelPack");

    _activeLevel = levelPack->ToElement()->FirstChild();
    _levelList.clear();
    while (_activeLevel) {
        _levelList.push_back(_activeLevel);
        _activeLevel = _activeLevel->NextSibling();
    }

    bool randomLevels = false;
    ConfigS::instance()->getBoolean("randomLevels", randomLevels);
    if (randomLevels) {
        int levelCount = _levelList.size();
        for (int i = 0; i < levelCount * 10; i++) {
            int r1 = _random.random() % levelCount;
            int r2 = _random.random() % levelCount;
            std::swap(_levelList[r1], _levelList[r2]);
        }
    }
    _activeLevelIndex = 0;
    _activeLevel = _levelList[_activeLevelIndex];

    //advance to next level pack
    _levelPackIterator++;

    if (!_activeLevelPack) {
        LOG_ERROR << "No level pack found!" << endl;
        return false;
    }

    return true;
}

bool StageManager::activateLevel(void) {
    XTRACE();

    if (!_activeLevel) {
        return false;
    }

    GameState::stopwatch.pause();

    TiXmlElement* levelNode = _activeLevel->ToElement();
    _activeLevelName = levelNode->Attribute("Name");
    LOG_INFO << "Level '" << _activeLevelName << "' by " << levelNode->Attribute("Author") << endl;

    static ParticleGroup* effects = ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);
    ParticleInfo pi;
    pi.position.x = 0.0;
    pi.position.y = 0.0;
    pi.position.z = -50.0;
    pi.text = _activeLevelName;

    pi.color.x = 1.0;
    pi.color.y = 1.0;
    pi.color.z = 1.0;

    pi.extra.y = 0.1f;
    pi.extra.z = 0.1f;

    effects->newParticle("StatusMessage", pi);

    GameState::numObjects = 0;
    _levelStartTime = GameState::stopwatch.getTime();
    _delayEndOfLevel = 60;  //2 sec

    TiXmlNode* enemyNode = _activeLevel->FirstChild();
    while (enemyNode) {
        EnemyFactory::createEnemy(enemyNode);
        enemyNode = enemyNode->NextSibling();
    }

    GameState::startOfStep = Timer::getTime();  //get fresh start for other logic

    GameState::stopwatch.start();
    LOG_INFO << "Starting game timer\n";

    return true;
}
