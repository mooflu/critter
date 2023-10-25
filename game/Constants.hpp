#pragma once
// Description:
//   All kinds of constants.
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

#ifdef HAVE_CONFIG_H
#include <defines.h>  //PACKAGE and VERSION
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#define GAME_HAS_HERO_PARTICLE 1

const std::string GAMETITLE = PACKAGE;
const std::string GAMEVERSION = VERSION;

const std::string HERO_GROUP = "Hero";
const std::string HERO_BULLETS_GROUP = "HeroBullets";
const std::string ENEMIES_GROUP = "Enemies";
const std::string ENEMIES_BULLETS_GROUP = "EnemyBullets";
const std::string BONUS_GROUP = "Bonus";
const std::string SHOOTABLE_ENEMY_BULLETS_GROUP = "ShootableEnemyBullets";
const std::string SHOOTABLE_BONUS_GROUP = "ShootableBonus";
const std::string EFFECTS_GROUP1 = "Effects1";
const std::string EFFECTS_GROUP2 = "Effects2";
const std::string EFFECTS_GROUP3 = "Effects3";

const int MAX_PARTICLES_PER_GROUP = 2048;

const float GAME_STEP_SIZE = 1.0f / 30.0f;  //run logic 30 times per second
const int MAX_GAME_STEPS = 10;              //max number of logic runs per frame

// All updates in out logic are based on a game step size of 1/30.
// In case we want to use a different GAME_STEP_SIZE in the future,
// multiply all update values by GAME_STEP_SCALE.
const float GAME_STEP_SCALE = 30.0f * GAME_STEP_SIZE;
