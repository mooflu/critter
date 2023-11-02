// Description:
//   Boss1 enemy controller.
//
// Copyright (C) 2005 Frank Becker
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
#include "Boss1Enemy.hpp"

#include "Point.hpp"
#include "GameState.hpp"
#include "Constants.hpp"
#include "RandomKnuth.hpp"
#include "LevelPack.hpp"
#include "ParticleType.hpp"
#include "ParticleGroup.hpp"
#include "ParticleGroupManager.hpp"
#include "StageManager.hpp"
#include "Audio.hpp"
#include "ScoreKeeper.hpp"
#include "ModelManager.hpp"

#include <GL/glew.h>
#include "gl3/Program.hpp"
#include "gl3/ProgramManager.hpp"
#include "gl3/MatrixStack.hpp"
#include "glm/ext.hpp"
#include "GLVertexBufferObject.hpp"

static RandomKnuth _random;

Boss1Enemy::Boss1Enemy(LEnemy* le) :
    BaseEnemy(le, "Boss1Enemy") {
    XTRACE();

    ModelManager& mm = *ModelManagerS::instance();
    string fullModelName = "models/" + _lEnemy->modelName;
    _model[0] = mm.getModel(fullModelName + "_Leg1");
    _model[1] = mm.getModel(fullModelName + "_Leg2");
    _model[2] = mm.getModel(fullModelName + "_Eye1");
    _model[3] = mm.getModel(fullModelName + "_Eye2");
    _model[4] = mm.getModel(fullModelName + "_Face");
    _model[5] = mm.getModel(fullModelName + "_Teeth");

    for (int i = 0; i < 6; i++) {
        _offsets[i] = _model[i]->getOffset();
    }

    norm(_dir);
    _energy = 10000;
    _enemyGroup->newParticle(this, _offset.x, _offset.y, 0);
}

Boss1Enemy::~Boss1Enemy() {
    XTRACE();
}

int Boss1Enemy::getRadiiCount(void) {
    return 6;
}

float Boss1Enemy::getRadius(int radIndex) {
    return _radii[radIndex];
}

vec3 Boss1Enemy::getOffset(int radIndex) {
    return _offsets[radIndex];
}

void Boss1Enemy::init(ParticleInfo* p) {
    //    XTRACE();
    p->position.z = -100.0;
    p->extra.x = 0;
    p->extra.y = 0;

    _lastLeft = 0;
    _lastRight = 0;

    p->damage = 1000;
    vec3 minpt, maxpt;
    p->radius = 0.0f;
    for (int i = 0; i < 6; i++) {
        _model[i]->getBoundingBox(minpt, maxpt);
        _radii[i] = (maxpt.x - minpt.x) / 1.8;  //2.0; make badies a little easied to hit
    }
    p->tod = -1;

    updatePrevs(p);
}

bool Boss1Enemy::update(ParticleInfo* p) {
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
    }

    _activePath->getPos(dt, _pos);
    _activePath->getTangent(dt, _dir);
    norm(_dir);

    ///LOG_INFO << "dt = " << dt << ", len = " << len << endl;
    ///LOG_INFO << "-p1 = " << _pos.x << "," << _pos.y << endl;
    ///LOG_INFO << "-off = " << _offset.x << "," << _offset.y << endl;

    updatePrevs(p);

    p->extra.y += 5.0f * GAME_STEP_SCALE;
    p->position.x = _pos.x + _offset.x;
    p->position.y = _pos.y + _offset.y;
    //	p->position.z = _pos.z + _offset.z;

    if (_extraUpdateAfterTransport) {
        //We transport the enemy to a staging area. We need to update again
        //to make sure we use the new position in the interpolation step
        updatePrevs(p);
        _extraUpdateAfterTransport = false;
    }

    static ParticleGroup* ebg = ParticleGroupManagerS::instance()->getParticleGroup(ENEMIES_BULLETS_GROUP);

    if ((_random.random() & 0xff) < Skill::getFireProbability()) {
        ParticleInfo pi;
        int whichLeg = (_random.random() & 8) >> 3;
        pi.position = p->position + _offsets[whichLeg];
        if (whichLeg == 1) {
            pi.position = pi.position + vec3(5, -5, 0);
            pi.velocity.x = 5.0f * 0.2f * GAME_STEP_SCALE;
            pi.velocity.y = -5.0f * 0.2f * GAME_STEP_SCALE;
            pi.velocity.z = 0.0f * 0.2f * GAME_STEP_SCALE;
            _lastLeft = ebg->newParticle("SwarmLeader", pi);
        } else {
            pi.position = pi.position + vec3(-5, -5, 0);
            pi.velocity.x = -5.0f * 0.2f * GAME_STEP_SCALE;
            pi.velocity.y = -5.0f * 0.2f * GAME_STEP_SCALE;
            pi.velocity.z = 0.0f * 0.2f * GAME_STEP_SCALE;
            _lastRight = ebg->newParticle("SwarmLeader", pi);
        }
        AudioS::instance()->playSample("sounds/puididl.wav");
    }

    if ((_random.random() & 0x1f) < Skill::getFireProbability()) {
        ParticleInfo pi;

        if (_lastLeft) {
            pi.related = _lastLeft;

            vec3 off2(1, 1, 0);
            off2 = off2 * ((_random.rangef0_1() - 0.5) * 5);
            pi.position = p->position + _offsets[1] + vec3(5, -5, 0) + off2;
            pi.velocity = vec3(0.7, -0.7, 0);

            ebg->newParticle("SwarmElement", pi);
        }

        if (_lastRight) {
            pi.related = _lastRight;

            vec3 off2(-1, 1, 0);
            off2 = off2 * ((_random.rangef0_1() - 0.5) * 5);
            pi.position = p->position + _offsets[0] + vec3(-5, -5, 0) + off2;
            pi.velocity = vec3(-0.7, -0.7, 0);

            ebg->newParticle("SwarmElement", pi);
        }
    }

    return true;
}

void Boss1Enemy::draw(ParticleInfo* p) {
    //    XTRACE();
    if (_enableDraw) {
        ParticleInfo pi;
        interpolate(p, pi);

        MatrixStack::model.push(MatrixStack::model.top());
        glm::mat4& modelview = MatrixStack::model.top();
        glm::mat4& projection = MatrixStack::projection.top();
        modelview = glm::translate(modelview, glm::vec3(pi.position.x, pi.position.y, pi.position.z));

        glEnable(GL_DEPTH_TEST);
        for (int i = 0; i < 6; i++) {
            MatrixStack::model.push(MatrixStack::model.top());
            glm::mat4& modelview = MatrixStack::model.top();
            modelview = glm::translate(modelview, glm::vec3(_offsets[i].x, _offsets[i].y, _offsets[i].z));

            _model[i]->draw();

            MatrixStack::model.pop();
        }
        glDisable(GL_DEPTH_TEST);

        float health = _energy / 1000.0f;
        float maxHealth = 10.0f;
        float tx = -5;
        float ty = 24;
        {
            GLVBO vbo;

            vec4f v2[4] = {
                vec4f(tx, ty + 2, 0, 1),
                vec4f(tx + maxHealth, ty + 2, 0, 1),
                vec4f(tx + maxHealth, ty + 3, 0, 1),
                vec4f(tx, ty + 3, 0, 1),
            };
            vbo.setColor(0.5f, 0.5f, 0.5f, 0.5f);
            vbo.DrawQuad(v2);

            vec4f v[4] = {
                vec4f(tx, ty + 2, 0, 1),
                vec4f(tx + health, ty + 2, 0, 1),
                vec4f(tx + health, ty + 3, 0, 1),
                vec4f(tx, ty + 3, 0, 1),
            };
            vbo.setColor(1.0f, 0.1f, 0.1f, 1.0f);
            vbo.DrawQuad(v);
        }

        MatrixStack::model.pop();
    }
}

void Boss1Enemy::hit(ParticleInfo* p, ParticleInfo* p2, int /*radIndex*/) {
    //    XTRACE();
    static ParticleGroup* effects = ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);

    int scoreAdd = 50;
    if (p2->damage > 400) {
        _energy -= 10;  //workaround for FlankBuster killer
        scoreAdd = 10;
    } else {
        _energy -= p2->damage;
    }

    if ((_energy <= 0) && (p->tod != 0)) {
        static ParticleGroup* bonus = ParticleGroupManagerS::instance()->getParticleGroup(BONUS_GROUP);

        p->tod = 0;
        int newValue = ScoreKeeperS::instance()->addToCurrentScore(5000);

        ParticleInfo pi;
        pi.position = p->position;
        char buf[10];
        sprintf(buf, "%d", newValue);
        pi.text = buf;

        pi.color.x = 1.0f;
        pi.color.y = 1.0f;
        pi.color.z = 1.0f;

        effects->newParticle("ScoreHighlight", pi);

        bonus->newParticle("SuperBonus", p->position.x, p->position.y, p->position.z);

        for (int j = 0; j < 6; j++) {
            vec3 pos = p->position + _offsets[j];
            //spawn explosion
            for (int i = 0; i < (int)(GameState::horsePower / 2.0); i++) {
                effects->newParticle("ExplosionPiece", pos.x, pos.y, pos.z);
            }
        }
        AudioS::instance()->playSample("sounds/explosion.wav");
    } else {
        //a hit but no kill
        ScoreKeeperS::instance()->addToCurrentScore(scoreAdd);

        ParticleInfo pi;
        pi.position = p2->position;

        pi.velocity.x = _dir.x * (0.6f * GAME_STEP_SCALE);
        pi.velocity.y = _dir.y * (0.6f * GAME_STEP_SCALE);
        pi.velocity.z = 0;

        effects->newParticle("MiniSmoke", pi);
    }
}
