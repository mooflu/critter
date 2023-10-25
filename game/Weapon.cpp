// Description:
//   All kinds of Weapons.
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
#include "Weapon.hpp"

#include <Trace.hpp>
#include <Audio.hpp>

#include <Constants.hpp>
#include <ParticleGroup.hpp>
#include <ParticleGroupManager.hpp>

#include <ModelManager.hpp>
#include <ResourceManager.hpp>
#include <zrwops.hpp>

using namespace std;

//------------------------------------------------------------------------------

Weapon::Weapon(float energyRequired, float reloadTime, string name, string desc) :
    _energyRequired(energyRequired),
    _reloadTime(reloadTime),
    _damageMultiplier(1.0f),
    _name(name),
    _description(desc) {
    XTRACE();
}

Weapon::~Weapon() {
    XTRACE();
}

float Weapon::energyRequired(void) {
    return (_energyRequired);
}

float Weapon::reloadTime(void) {
    return (_reloadTime);
}

void Weapon::setDamageMultiplier(float damageMultiplier) {
    _damageMultiplier = damageMultiplier;
}

//------------------------------------------------------------------------------

GuardDog::GuardDog(void) :
    Weapon(0, 5, WeaponDOG, "This dog may bite!") {
    XTRACE();
}

GuardDog::~GuardDog() {
    XTRACE();
}

void GuardDog::launch(float /*x*/, float /*y*/, float /*z*/) {
    XTRACE();
    AudioS::instance()->playSample("sounds/bark.wav");
    LOG_INFO << "...<Bark>... :-)" << endl;
}

//------------------------------------------------------------------------------

Stinger::Stinger(void) :
    Weapon(30.0f, 0.20f, WeaponSTINGER, "Not much punch, but keep your hamster out of the way!") {
    XTRACE();
    _stinger = ModelManagerS::instance()->getModel("models/Stinger");
}

Stinger::~Stinger() {
    XTRACE();
}

void Stinger::launch(float x, float y, float z) {
    XTRACE();
    AudioS::instance()->playSample("sounds/laser.wav");
    //        LOG_INFO << "Launching stinger..." << endl;
    static ParticleGroup* bullets = ParticleGroupManagerS::instance()->getParticleGroup(HERO_BULLETS_GROUP);
    bullets->newParticle(name(), x, y, z);
}

void Stinger::draw(void) {
    _stinger->draw();
}

//------------------------------------------------------------------------------

FlankBurster::FlankBurster(void) :
    Weapon(80.0, 2.0, WeaponFLANKBURST, "Mr Proper says: it cleans your flanks, too!") {
    XTRACE();
    _flankBurster = ModelManagerS::instance()->getModel("models/FlankBurster");
}

FlankBurster::~FlankBurster() {
    XTRACE();
}

void FlankBurster::launch(float x, float y, float z) {
    XTRACE();
    AudioS::instance()->playSample("sounds/ieow.wav");
    static ParticleGroup* bullets = ParticleGroupManagerS::instance()->getParticleGroup(HERO_BULLETS_GROUP);
    string left = name() + "Left";
    //    LOG_INFO << "Launching Flank Bursters.." << left << endl;
    bullets->newParticle(left, x, y, z);

    string right = name() + "Right";
    //    LOG_INFO << "Launching Flank Bursters.." << right << endl;
    bullets->newParticle(right, x, y, z);
}

void FlankBurster::draw(void) {
    _flankBurster->draw();
}

//------------------------------------------------------------------------------

IceSpray::IceSpray(void) :
    Weapon(45.0f, 0.4f, WeaponICESPRAY, "Shiver! Beware any shrinkage...") {
    XTRACE();
    _iceSpray = ModelManagerS::instance()->getModel("models/IceSpray");
    _iceSprayPierce = ModelManagerS::instance()->getModel("models/IceSprayPierce");
}

IceSpray::~IceSpray() {
    XTRACE();
}

void IceSpray::launch(float x, float y, float z) {
    XTRACE();

    static ParticleGroup* bullets = ParticleGroupManagerS::instance()->getParticleGroup(HERO_BULLETS_GROUP);

    for (int i = 0; i < 5; i++) {
        ParticleInfo pi;
        pi.position.x = x;
        pi.position.y = y;
        pi.position.z = z;

        pi.velocity.z = 0.0f * GAME_STEP_SCALE;

        switch (i) {
            case 0:
                pi.velocity.x = 0.0f * GAME_STEP_SCALE;
                pi.velocity.y = 2.0f * GAME_STEP_SCALE;
                break;

            case 1:
                pi.velocity.x = 1.0f * GAME_STEP_SCALE;
                pi.velocity.y = 1.732f * GAME_STEP_SCALE;
                break;

            case 2:
                pi.velocity.x = -1.0f * GAME_STEP_SCALE;
                pi.velocity.y = 1.732f * GAME_STEP_SCALE;
                break;

            case 3:
                pi.velocity.x = 1.732f * GAME_STEP_SCALE;
                pi.velocity.y = 1.0f * GAME_STEP_SCALE;
                break;

            case 4:
                pi.velocity.x = -1.732f * GAME_STEP_SCALE;
                pi.velocity.y = 1.0f * GAME_STEP_SCALE;
                break;

            default:
                break;
        }

        pi.damage = (int)(50.0f * _damageMultiplier);

        if (pi.damage < 100) {
            _energyRequired = 45.0;
            bullets->newParticle("BallOfIce", pi);
        } else {
            _energyRequired = 60.0;
            bullets->newParticle("BallOfPoison", pi);
        }
    }

    AudioS::instance()->playSample("sounds/chrblub.wav");
}

void IceSpray::draw(void) {
    if (_damageMultiplier > 1.0f) {
        _iceSprayPierce->draw();
    } else {
        _iceSpray->draw();
    }
}

//------------------------------------------------------------------------------

WingPhaser::WingPhaser(void) :
    Weapon(35.0f, 0.3f, WeaponWINGPHASER, "A wing-mounted double blow.") {
    XTRACE();
    _wingPhaser = ModelManagerS::instance()->getModel("models/WingPhaser");
    _wingPhaserPierce = ModelManagerS::instance()->getModel("models/WingPhaserPierce");
}

WingPhaser::~WingPhaser() {
    XTRACE();
}

void WingPhaser::launch(float x, float y, float z) {
    XTRACE();

    static ParticleGroup* bullets = ParticleGroupManagerS::instance()->getParticleGroup(HERO_BULLETS_GROUP);

    ParticleInfo pi;
    pi.damage = (int)(50.0f * _damageMultiplier);

    if (pi.damage < 100) {
        pi.color.x = 0.0f;
        pi.color.y = 1.0f;
        pi.color.z = 1.0f;

        _energyRequired = 25.0f;
        _reloadTime = 0.2f;
    } else {
        pi.color.x = 1.0f;
        pi.color.y = 0.2f;
        pi.color.z = 0.2f;

        _energyRequired = 35.0f;
        _reloadTime = 0.3f;
    }

    pi.position.x = x + 4.2f;
    pi.position.y = y;
    pi.position.z = z;
    bullets->newParticle("WingPhaser1", pi);

    pi.position.x = x - 4.2f;
    pi.position.y = y;
    pi.position.z = z;
    bullets->newParticle("WingPhaser1", pi);

    AudioS::instance()->playSample("sounds/phaser1.wav");
}

void WingPhaser::draw(void) {
    if (_damageMultiplier > 1.0f) {
        _wingPhaserPierce->draw();
    } else {
        _wingPhaser->draw();
    }
}

//------------------------------------------------------------------------------

TripplePhaser::TripplePhaser(void) :
    Weapon(30.0f, 0.2f, WeaponTRIPPLEPHASER, "A wing and cockpit mounted tripple blow.") {
    XTRACE();
    _tripplePhaser = ModelManagerS::instance()->getModel("models/WingPhaser");
}

TripplePhaser::~TripplePhaser() {
    XTRACE();
}

void TripplePhaser::launch(float x, float y, float z) {
    XTRACE();

    static ParticleGroup* bullets = ParticleGroupManagerS::instance()->getParticleGroup(HERO_BULLETS_GROUP);

    ParticleInfo pi;
    pi.damage = (int)(100.0f * _damageMultiplier);

    pi.color.x = 1.0f;
    pi.color.y = 1.0f;
    pi.color.z = 0.2f;

    pi.position.x = x + 4.2f;
    pi.position.y = y;
    pi.position.z = z;
    bullets->newParticle("WingPhaser1", pi);

    pi.position.x = x;
    pi.position.y = y + 4.5f;
    pi.position.z = z;
    bullets->newParticle("WingPhaser1", pi);

    pi.position.x = x - 4.2f;
    pi.position.y = y;
    pi.position.z = z;
    bullets->newParticle("WingPhaser1", pi);

    AudioS::instance()->playSample("sounds/phaser1");
}

void TripplePhaser::draw(void) {
    _tripplePhaser->draw();
}

//------------------------------------------------------------------------------
