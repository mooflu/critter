// Description:
//   Particle group manager.
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
#include "ParticleGroupManager.hpp"

#include <Trace.hpp>

#include <ParticleGroup.hpp>
#include <FindHash.hpp>

using namespace std;

ParticleGroupManager::ParticleGroupManager( void)
{
    XTRACE();
}

ParticleGroupManager::~ParticleGroupManager()
{
    XTRACE();

    list<LinkedParticleGroup*>::iterator li;
    for( li=_linkedParticleGroupList.begin(); 
         li!=_linkedParticleGroupList.end(); li++)
    {
        delete (*li);
    }
    _linkedParticleGroupList.clear();

    list<ParticleGroup*>::iterator i;
    for( i=_particleGroupList.begin(); i!=_particleGroupList.end(); i++)
    {
	delete (*i);
    }

    ParticleGroup::destroyParticleTypes();

    _particleGroupMap.clear();
    _particleGroupList.clear();
}

bool ParticleGroupManager::init( void)
{
    XTRACE();
    return true;
}

void ParticleGroupManager::reset( void)
{
    XTRACE();

    list<ParticleGroup*>::iterator i;
    for( i=_particleGroupList.begin(); i!=_particleGroupList.end(); i++)
    {
	(*i)->reset();
    }
}

void ParticleGroupManager::draw( void)
{
    XTRACE();

    list<ParticleGroup*>::iterator i;
    for( i=_particleGroupList.begin(); i!=_particleGroupList.end(); i++)
    {
	(*i)->draw();
    }
}

bool ParticleGroupManager::update( void)
{
    XTRACE();

    list<ParticleGroup*>::iterator i;
    for( i=_particleGroupList.begin(); i!=_particleGroupList.end(); i++)
    {
        (*i)->update();
    }

    list<LinkedParticleGroup*>::iterator li;
    for( li=_linkedParticleGroupList.begin(); 
         li!=_linkedParticleGroupList.end(); li++)
    {
	LinkedParticleGroup *lpg = *li;
        //collision detect based on links...
        lpg->group1->detectCollisions( lpg->group2);
    }

    return true;
}

void ParticleGroupManager::addGroup( const string &groupName, int groupSize)
{
    XTRACE();
    ParticleGroup *newGroup = new ParticleGroup( groupName, groupSize);
    newGroup->init();
    _particleGroupMap[ groupName] = newGroup;

    //We keep a list in addition to the hash map in order to have a bit
    //more control over drawing order.
    _particleGroupList.insert( _particleGroupList.end(), newGroup);

    LOG_INFO << "Adding Particle Group [" << groupName << "]" << endl;
}

void ParticleGroupManager::addLink( const string &group1, const string &group2)
{
    XTRACE();

    ParticleGroup *g1 = getParticleGroup( group1);
    ParticleGroup *g2 = getParticleGroup( group2);

    if( !g1 || !g2) return;

    LOG_INFO << "Collision detection between [" 
             << group1 << "] and [" 
             << group2 << "] enabled." 
             << endl;

    LinkedParticleGroup *lpg = new LinkedParticleGroup;
    lpg->group1 = g1;
    lpg->group2 = g2;

    _linkedParticleGroupList.insert( _linkedParticleGroupList.end(), lpg);
}

ParticleGroup *ParticleGroupManager::getParticleGroup( const string &groupName)
{
    XTRACE();
    ParticleGroup * pg = findHash<const string>( groupName, _particleGroupMap);
    if( !pg)
    {
        LOG_ERROR << "Unable to find particle group " << groupName << endl;
    }
    return pg;
}

int ParticleGroupManager::getAliveCount( void)
{
    XTRACE();
    int total = 0;
    list<ParticleGroup*>::iterator i;
    for( i=_particleGroupList.begin(); i!=_particleGroupList.end(); i++)
    {
        total += (*i)->getAliveCount();
    }
    return total;
}
