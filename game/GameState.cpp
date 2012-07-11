// Description:
//   Collection of game state variables.
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
#include <GameState.hpp>

bool GameState::isDeveloper = false;
bool GameState::showFPS = false;
bool GameState::isAlive = true;
float GameState::startOfStep = 0;
float GameState::frameFractionOther = 0.0;
float GameState::startOfGameStep = 0;
float GameState::frameFraction = 0.0;
float GameState::horsePower = 100.0;
int GameState::numObjects = 0;
int GameState::enemyBulletCount = 0;
int GameState::enemyAttackCount = 0;
Context::ContextEnum GameState::context = Context::eUnknown;
PausableTimer GameState::stopwatch;
Skill::SkillEnum GameState::skill = Skill::eUnknown;
