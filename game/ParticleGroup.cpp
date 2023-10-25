// Description:
//   Particles are grouped into ParticleGroups.
//
// Copyright (C) 2008 Frank Becker
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
#include "ParticleGroup.hpp"

#include <Trace.hpp>
#include <Particles.hpp>
#include <Constants.hpp>
#include <FindHash.hpp>

using namespace std;

hash_map<const string, ParticleType*, hash<const string>, std::equal_to<const string>> ParticleGroup::_particleTypeMap;

ParticleGroup::ParticleGroup(const string& groupName, int numParticles) :
    _particles(0),
    _aliveCount(0),
    _groupName(groupName),
    _numParticles(numParticles) {
    XTRACE();
    _usedList.next = 0;
}

ParticleGroup::~ParticleGroup() {
    XTRACE();

    LOG_INFO << _groupName << " has " << _aliveCount << " particles still alive.\n";
    ParticleInfo* p = _usedList.next;
    while (p) {
        p->tod = 0;
        p->particle->update(p);
        p = p->next;
    }

    delete[] _particles;
}

void ParticleGroup::reset(void) {
    XTRACE();

    //trigger particles in use to die
    ParticleInfo* p = _usedList.next;
    while (p) {
        p->tod = 0;
        p->particle->update(p);
        p = p->next;
    }

    //reset free/used lists
    _freeList.next = &_particles[0];
    for (int i = 0; i < _numParticles - 1; i++) {
        _particles[i].next = &_particles[i + 1];
    }
    _particles[_numParticles - 1].next = 0;
    _usedList.next = 0;
    _aliveCount = 0;
}

bool ParticleGroup::init(void) {
    XTRACE();

    //FIXME: do we really need to restrict size?
    if (_numParticles > MAX_PARTICLES_PER_GROUP) {
        _numParticles = MAX_PARTICLES_PER_GROUP;
    }

    _particles = new ParticleInfo[_numParticles];
    for (int i = 0; i < _numParticles - 1; i++) {
        const vec3 zero(0, 0, 0);
        _particles[i].position = zero;
        _particles[i].velocity = zero;
        _particles[i].color = zero;
        _particles[i].extra = zero;

        _particles[i].prevPosition = zero;
        _particles[i].prevVelocity = zero;
        _particles[i].prevColor = zero;
        _particles[i].prevExtra = zero;
    }

    reset();

    Particles::Initialize();

    return true;
}

ParticleInfo* ParticleGroup::newParticle(const string& name, const ParticleInfo& pi) {
    //    XTRACE();
    ParticleType* particleType = getParticleType(name);
    if (!particleType) {
        LOG_ERROR << "Unknown particleType [" << name << "]" << endl;
        return 0;
    }
    return newParticle(particleType, pi);
}

ParticleInfo* ParticleGroup::newParticle(ParticleType* particleType, const ParticleInfo& pi) {
    //    XTRACE();
    ParticleInfo* p = _freeList.next;
    if (!p) {
        LOG_ERROR << _groupName << " is out of particles!" << endl;
        return 0;
    }

    _freeList.next = p->next;
    p->next = _usedList.next;
    _usedList.next = p;

    p->particle = particleType;
    p->position = pi.position;
    p->velocity = pi.velocity;
    p->extra = pi.extra;
    p->color = pi.color;
    p->text = pi.text;
    p->damage = pi.damage;
    p->related = pi.related;

    //particle initializes particle info
    particleType->init(p);

    _aliveCount++;

    return p;
}

ParticleInfo* ParticleGroup::newParticle(const string& name, float x, float y, float z) {
    //    XTRACE();
    ParticleType* particleType = getParticleType(name);
    if (!particleType) {
        LOG_ERROR << "Unknown particleType [" << name << "]" << endl;
        return 0;
    }
    return newParticle(particleType, x, y, z);
}

ParticleInfo* ParticleGroup::newParticle(ParticleType* particleType, float x, float y, float z) {
    //    XTRACE();
    ParticleInfo* p = _freeList.next;
    if (!p) {
        LOG_ERROR << _groupName << " is out of particles!" << endl;
        return 0;
    }

    _freeList.next = p->next;
    p->next = _usedList.next;
    _usedList.next = p;

    p->particle = particleType;
    p->position.x = x;
    p->position.y = y;
    p->position.z = z;

    //particle initializes particle info
    particleType->init(p);

    _aliveCount++;

    return p;
}

static inline bool hasRadiusCollision(const float& minDist, const vec3& pos1, const vec3& pos2) {
    float d2 = minDist;
    d2 *= d2;
    float dx = (pos2.x - pos1.x);
    float dy = (pos2.y - pos1.y);
    float D2 = dx * dx + dy * dy;
    if (D2 < d2) {
        return true;
    }
    return false;
}

void ParticleGroup::detectCollisions(ParticleGroup* pg) {
    //    XTRACE();
    ParticleInfo* p1 = _usedList.next;
    while (p1) {
        ParticleInfo* p2 = pg->_usedList.next;
        while (p2) {
            if ((p1->radius == 0.0f) || (p2->radius == 0.0f)) {
                //Bosses are made up of multiple radii
                for (int rc1 = 0; rc1 < p1->particle->getRadiiCount(); rc1++) {
                    float r1 = p1->particle->getRadius(rc1);
                    if (r1 == 0.0f) {
                        r1 = p1->radius;
                    }
                    vec3 offset1 = p1->particle->getOffset(rc1);
                    for (int rc2 = 0; rc2 < p2->particle->getRadiiCount(); rc2++) {
                        float r2 = p2->particle->getRadius(rc2);
                        if (r2 == 0.0f) {
                            r2 = p2->radius;
                        }
                        vec3 offset2 = p2->particle->getOffset(rc2);
                        vec3 pos1 = p1->position + offset1;
                        vec3 pos2 = p2->position + offset2;
                        //			LOG_INFO << "pos1 = " << pos1.x << " " << pos1.y << "\n";
                        //			LOG_INFO << "pos2 = " << pos2.x << " " << pos2.y << "\n";
                        if (hasRadiusCollision(r1 + r2, pos1, pos2)) {
                            p1->particle->hit(p1, p2, rc1);
                            p2->particle->hit(p2, p1, rc2);
                        }
                    }
                }
            } else {
                if (hasRadiusCollision(p1->radius + p2->radius, p1->position, p2->position)) {
                    p1->particle->hit(p1, p2);
                    p2->particle->hit(p2, p1);
                }
            }

            p2 = p2->next;
        }

        p1 = p1->next;
    }
}

void ParticleGroup::addParticleType(ParticleType* particleType) {
    XTRACE();
    if (particleType) {
        LOG_INFO << "New Particle type: [" << particleType->name() << "]" << endl;

        _particleTypeMap[particleType->name()] = particleType;
    }
}

ParticleType* ParticleGroup::getParticleType(const string particleTypeName) {
    //    XTRACE();
    ParticleType* particleType = findHash<const string>(particleTypeName, _particleTypeMap);
    if (!particleType) {
        LOG_ERROR << "ParticleGroup never heard of a " << particleTypeName << " before!" << endl;

        //trying first in list
        particleType = _particleTypeMap.begin()->second;
    }
    return particleType;
}
