// Description:
//   Basic enemy controller. Controls enemy movement along paths.
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
#include <Trace.hpp>
#include <SimpleEnemy.hpp>

#include <Point.hpp>
#include <Constants.hpp>
#include <GameState.hpp>
#include <RandomKnuth.hpp>
#include <LevelPack.hpp>
#include <ParticleType.hpp>
#include <ParticleGroup.hpp>
#include <ParticleGroupManager.hpp>
#include <StageManager.hpp>
#include <Audio.hpp>
#include <ScoreKeeper.hpp>
#include <ModelManager.hpp>

#include <GL/glew.h>
#include "gl3/Program.hpp"
#include "gl3/ProgramManager.hpp"
#include "gl3/MatrixStack.hpp"
#include "glm/ext.hpp"

static RandomKnuth _random;

//Rolling is currently disabled since it looks totally crappy in some
//situations. The idea was for the bugs to lean into the curve (like
//a motorcycle for example)..
#define WITH_ROLLING 0

SimpleEnemy::SimpleEnemy(LEnemy* le) :
    BaseEnemy(le, "SimpleEnemy") {
    XTRACE();

    ModelManager& mm = *ModelManagerS::instance();
    string fullModelName = "models/" + _lEnemy->modelName;
    _model = mm.getModel(fullModelName);

#ifdef WITH_ROLLING
    norm(_dir);
#endif
    _enemyGroup->newParticle(this, _offset.x, _offset.y, 0);
}

SimpleEnemy::~SimpleEnemy() {
    XTRACE();
}

void SimpleEnemy::init(ParticleInfo* p) {
    //    XTRACE();
    p->position.z = -100.0;
    p->extra.x = 0;
    p->extra.y = 0;

    p->damage = 100;
    vec3 minpt, maxpt;
    _model->getBoundingBox(minpt, maxpt);
    p->radius = (maxpt.x - minpt.x) / 1.8;  //2.0; make badies a little easied to hit
    p->tod = -1;

    updatePrevs(p);
}

bool SimpleEnemy::update(ParticleInfo* p) {
    //    XTRACE();
    if (p->tod == 0) {
        GameState::numObjects--;
        if ((_moveState == eAttack) || (_moveState == eRetreat)) {
            GameState::enemyAttackCount--;
        }
        delete this;
        return false;
    }

    float dt = GameState::startOfGameStep - _pathStartTime;
    if (dt < 0) {
        return true;
    }
    _enableDraw = true;

    float len = _activePath->length();

    _prevDir = _dir;
    if (dt > len) {
        //LOG_INFO << "dt = " << dt << ", len = " << len << endl;
        getNextPath();
        dt -= len;
        _pathStartTime += len;
        _activePath->getTangent(dt, _prevDir);
    } else if (_moveHome || (p->position.y < -57.0)) {
        _moveHome = true;
        return true;
    }

    _activePath->getPos(dt, _pos);
    _activePath->getTangent(dt, _dir);

#ifdef WITH_ROLLING
    norm(_dir);
    if (_moveState != eIdle) {
        //The area of the triangle for the previous and current direction
        //vector is an indication of how sharp a turn the enemy is making.
        //Use this to roll the enemy.
        Point3D c = cross(_prevDir, _dir);
        float a = 250 * sqrt(dot(c, c));
        //is enemy turning left or right
        if (c.z < 0) {
            a = -a;
        }
        p->extra.y = a;
        //	LOG_INFO << "rot = " << p->extra.y << endl;
    } else {
        p->extra.y = 0.0;
    }
#endif

    ///LOG_INFO << "dt = " << dt << ", len = " << len << endl;
    ///LOG_INFO << "-p1 = " << _pos.x << "," << _pos.y << endl;
    ///LOG_INFO << "-off = " << _offset.x << "," << _offset.y << endl;

    updatePrevs(p);

    p->position.x = _pos.x + _offset.x;
    p->position.y = _pos.y + _offset.y;
    //	p->position.z = _pos.z + _offset.z;

    if (_extraUpdateAfterTransport) {
        //We transport the enemy to a staging area. We need to update again
        //to make sure we use the new position in the interpolation step
        updatePrevs(p);
        _extraUpdateAfterTransport = false;
    }

    static ParticleGroup* ebg = ParticleGroupManagerS::instance()->getParticleGroup(SHOOTABLE_ENEMY_BULLETS_GROUP);

    if ((GameState::enemyBulletCount < Skill::getMaxBullets()) &&
        ((_moveState == eIdle) || (_moveState == eAttack) || (GameState::skill != Skill::eRookie))) {
        p->extra.x += 0.1f * GAME_STEP_SCALE;
        if (p->extra.x > 0.5) {
            if ((_random.random() & 0xff) < Skill::getFireProbability()) {
                ebg->newParticle(
                    //		    "PlasmaBullet",p->position.x,p->position.y,p->position.z);
                    "BallOfFire", p->position.x, p->position.y, p->position.z);
                AudioS::instance()->playSample("sounds/laser3.wav");
            }
            p->extra.x = 0;
        }
    }

    return true;
}

void SimpleEnemy::draw(ParticleInfo* p) {
    //    XTRACE();
    if (_enableDraw) {
        ParticleInfo pi;
        interpolate(p, pi);

        MatrixStack::model.push(MatrixStack::model.top());
        glm::mat4& modelview = MatrixStack::model.top();
        modelview = glm::translate(modelview, glm::vec3(pi.position.x, pi.position.y, pi.position.z));

        if (_moveState != eIdle) {
            float gf = GameState::frameFraction;
            Point3D _dirInterp;
            _dirInterp.x = _prevDir.x + (_dir.x - _prevDir.x) * gf;
            _dirInterp.y = _prevDir.y + (_dir.y - _prevDir.y) * gf;
            _dirInterp.z = _prevDir.z + (_dir.z - _prevDir.z) * gf;

            alignWith(_dirInterp, modelview);
#if WITH_ROLLING
            modelview = glm::rotate(modelview, glm::radians(pi.extra.y), glm::vec3(0., 1., 0.));
#endif
        }
        glEnable(GL_DEPTH_TEST);

        _model->draw();

        glDisable(GL_DEPTH_TEST);

        MatrixStack::model.pop();
    }
}

void SimpleEnemy::hit(ParticleInfo* p, int damage, int /*radIndex*/) {
    //    XTRACE();
    static ParticleGroup* effects = ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);

    _energy -= damage;
    if (_energy <= 0) {
        static ParticleGroup* bonus = ParticleGroupManagerS::instance()->getParticleGroup(BONUS_GROUP);

        p->tod = 0;
        if (_moveState == eAttack) {
            ScoreKeeperS::instance()->addToCurrentScore(200);
        } else {
            ScoreKeeperS::instance()->addToCurrentScore(100);
        }

        if ((_random.random() & 0xff) > 10) {
            bonus->newParticle("Bonus1", p->position.x, p->position.y, p->position.z);
        } else {
            bonus->newParticle("SuperBonus", p->position.x, p->position.y, p->position.z);
        }

        //spawn explosion
        for (int i = 0; i < (int)(GameState::horsePower / 2.0); i++) {
            effects->newParticle("ExplosionPiece", p->position.x, p->position.y, p->position.z);
        }
        AudioS::instance()->playSample("sounds/explosion.wav");
    } else {
        if (damage > 0) {
            //a hit but no kill
            ScoreKeeperS::instance()->addToCurrentScore(50);
        }

        ParticleInfo pi;

        pi.position.x = p->position.x;
        pi.position.y = p->position.y;
        pi.position.z = p->position.z;

        pi.velocity.x = _dir.x * 0.6f * GAME_STEP_SCALE;
        pi.velocity.y = _dir.y * 0.6f * GAME_STEP_SCALE;
        pi.velocity.z = _dir.z * 0.6f * GAME_STEP_SCALE;

        effects->newParticle("MiniSmoke", pi);
    }
}
