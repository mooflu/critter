// Description:
//   Skill settings
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
#include "Skill.hpp"

#include <Trace.hpp>
#include <Config.hpp>
#include <GameState.hpp>
#include <Hero.hpp>

using namespace std;

Skill::Skill(void) {
    XTRACE();
    string skill = SKILL_NORMAL;
    ConfigS::instance()->getString("skill", skill);
    updateSkill(convertStringToSkill(skill));
}

Skill::~Skill() {
    XTRACE();
}

void Skill::updateSkill(const Skill::SkillEnum& skill) {
    XTRACE();

    HeroS::instance()->assignWeapons(skill);

    if (GameState::skill == skill) {
        return;
    }

    GameState::skill = skill;

    string newSkill = getString(GameState::skill);
    LOG_INFO << "Setting skill to " << newSkill << endl;

    ConfigS::instance()->updateKeyword("skill", newSkill);

    switch (GameState::skill) {
        case Skill::eRookie:
            _fireProbability = 7;  //0-255
            _maxBullets = 3;
            _maxAttacking = 1;
            break;

        case Skill::eNormal:
            _fireProbability = 10;  //0-255
            _maxBullets = 7;
            _maxAttacking = 3;
            break;

        case Skill::eExpert:
            _fireProbability = 15;  //0-255
            _maxBullets = 12;
            _maxAttacking = 7;
            break;

        case Skill::eInsane:
            _fireProbability = 25;  //0-255
            _maxBullets = 30;
            _maxAttacking = 50;
            break;

        default:
            _fireProbability = 0;  //0-255
            _maxBullets = 0;
            break;
    }
}

void Skill::updateSkill(void) {
    string newSkill;
    ConfigS::instance()->getString("skill", newSkill);
    updateSkill(convertStringToSkill(newSkill));
}

void Skill::incrementSkill(void) {
    if ((GameState::skill + 1) != Skill::eLAST) {
        updateSkill((Skill::SkillEnum)(GameState::skill + 1));
    }
}

Skill::SkillEnum Skill::convertStringToSkill(const string& skill) {
    XTRACE();
    if (skill == SKILL_ROOKIE) {
        return eRookie;
    }
    if (skill == SKILL_NORMAL) {
        return eNormal;
    }
    if (skill == SKILL_EXPERT) {
        return eExpert;
    }
    if (skill == SKILL_INSANE) {
        return eInsane;
    }

    return eNormal;
}
