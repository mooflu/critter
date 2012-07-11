#pragma once
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
#include <string>
#include <hashMap.hpp>

#include <HashString.hpp>
#include <ParticleInfo.hpp>
#include <ParticleType.hpp>
#ifdef GAME_HAS_HERO_PARTICLE
#include <Hero.hpp>
#endif

class ParticleGroup
{
public:
    ParticleGroup( const std::string &groupName, int numParticles);
    ~ParticleGroup();

    ParticleInfo *newParticle( const std::string &name, float x, float y, float z);
    ParticleInfo *newParticle( ParticleType *particleType, float x, float y, float z);

    ParticleInfo *newParticle( const std::string & name, const ParticleInfo &pi);
    ParticleInfo *newParticle( ParticleType *particleType, const ParticleInfo &pi);

    void update( void)
    {
    //    XTRACE();
	ParticleInfo *prev = &_usedList;
	ParticleInfo *p = _usedList.next;
	while( p)
	{
	    if( !p->particle->update( p))
	    {
		//particle is dead
		prev->next = p->next;

		ParticleInfo *tmp = p;
		p = p->next; 
	
		//put dead particle back in free list
		tmp->next = _freeList.next;
		_freeList.next = tmp;
	
		_aliveCount--;
		continue;
	    }
	    prev = p;
	    p = p->next;
	}
    }

    void draw( void)
    {
    //    XTRACE();
	ParticleInfo *p = _usedList.next;
	while( p)
	{
	    p->particle->draw( p);
	    p = p->next; 
	}
    }   

    bool init( void);
    void reset( void);

    int getAliveCount( void){ return _aliveCount;}

    //FIXME: find better place for this...
    void detectCollisions( ParticleGroup *pg);

    static void addParticleType( ParticleType *particleType);

    static void destroyParticleTypes( void)
    {
	hash_map< const std::string, ParticleType*, hash<const std::string> >::const_iterator ci;
	for( ci=_particleTypeMap.begin(); ci!=_particleTypeMap.end(); ci++)
	{
#ifdef GAME_HAS_HERO_PARTICLE
	    //don't delete hero!
	    if( ci->second != HeroS::instance()) 
#endif
		delete ci->second;
	}

	_particleTypeMap.clear();
    }

private:
    ParticleGroup( const ParticleGroup&);
    ParticleGroup &operator=(const ParticleGroup&);

    ParticleType * getParticleType( const std::string particleTypeName);

    //All Particle manager share the particleTypeMap
    static hash_map< 
        const std::string, ParticleType*, hash<const std::string>, std::equal_to<const std::string> > _particleTypeMap;

    ParticleInfo *_particles;

    ParticleInfo _freeList;
    ParticleInfo _usedList;

    int _aliveCount;
    std::string _groupName;
    int _numParticles;
};
