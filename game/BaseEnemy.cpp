// Description:
//   Basic enemy controller. Controls enemy movement along paths.
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
#include <Trace.hpp>
#include <BaseEnemy.hpp>

#include <Point.hpp>
#include <GameState.hpp>
#include <Constants.hpp>
#include <Random.hpp>
#include <LevelPack.hpp>
#include <ParticleType.hpp>
#include <ParticleGroup.hpp>
#include <ParticleGroupManager.hpp>
#include <StageManager.hpp>
#include <Audio.hpp>
#include <ScoreKeeper.hpp>

#include <GLee.h>

ParticleGroup *BaseEnemy::_enemyGroup = 0;

BaseEnemy::BaseEnemy( LEnemy *le, std::string name):
        ParticleType( name, false),
        _lEnemy(le),
	_energy(100),
        _enableDraw(false),
	_extraUpdateAfterTransport(false),
        _moveState(eEntry),
        _moveHome(false),
        _transitionBC( *new BezierCurve<Point3D>(4))
{
    XTRACE();
    if( !_enemyGroup)
    {
	_enemyGroup = ParticleGroupManagerS::instance()->getParticleGroup(
	    ENEMIES_GROUP);
    }

    GameState::numObjects++;

    _levelStartTime = 
        StageManagerS::instance()->levelStartTime() + le->spawnTime;
    //start offscreen
    _activePathIndex = 0;
    _activePath = le->entry;
    _activePath->getTangent( 0.0, _dir);
    _offset = le->spawnPoint;
    _pathStartTime = _levelStartTime;

    //bezier curve with 4 control points
    Point3D p1(0,0,0);
    _transitionBC.AddSegment(p1);
    _transitionBC.AddSegment(p1);

    _transitionPath.init( &_transitionBC);
}

BaseEnemy::~BaseEnemy()
{
    XTRACE();
    delete [] _lEnemy->attack;
    delete [] _lEnemy->retreat;
    delete _lEnemy;
}

bool BaseEnemy::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0)
    {
	GameState::numObjects--;
	if( (_moveState==eAttack) || (_moveState==eRetreat))
	{
	    GameState::enemyAttackCount--;
	}
	delete this;
	return false;
    }

    float dt = GameState::startOfGameStep - _pathStartTime;
    if( dt < 0)
    {
	return true;
    }
    _enableDraw = true;

    float len = _activePath->length();

    _prevDir = _dir;
    if( dt > len)
    {
//LOG_INFO << "dt = " << dt << ", len = " << len << endl;
	getNextPath();
	dt -= len;
	_pathStartTime += len;
	_activePath->getTangent( dt, _prevDir); 
    }

    _activePath->getPos( dt, _pos);
    _activePath->getTangent( dt, _dir); 

///LOG_INFO << "dt = " << dt << ", len = " << len << endl;
///LOG_INFO << "-p1 = " << _pos.x << "," << _pos.y << endl;
///LOG_INFO << "-off = " << _offset.x << "," << _offset.y << endl;

    updatePrevs(p);

    p->position.x = _pos.x + _offset.x;
    p->position.y = _pos.y + _offset.y;
//	p->position.z = _pos.z + _offset.z;

    if( _extraUpdateAfterTransport)
    {
	//We transport the enemy to a staging area. We need to update again 
	//to make sure we use the new position in the interpolation step
	updatePrevs(p);
	_extraUpdateAfterTransport = false;
    }

    static ParticleGroup *ebg=
	ParticleGroupManagerS::instance()->getParticleGroup( 
	    SHOOTABLE_ENEMY_BULLETS_GROUP);

    if( (GameState::enemyBulletCount < Skill::getMaxBullets()) && 
	( (_moveState == eIdle) || 
	  (_moveState == eAttack) ||
	  (GameState::skill!=Skill::eRookie)))
    {
	p->extra.x += 0.1f*GAME_STEP_SCALE;
	if( p->extra.x > 0.5)
	{
	    if( (Random::random() & 0xff) < Skill::getFireProbability())
	    {
		ebg->newParticle(
//		    "PlasmaBullet",p->position.x,p->position.y,p->position.z);
		    "BallOfFire",p->position.x,p->position.y,p->position.z);
		AudioS::instance()->playSample( "sounds/laser3.wav");
	    }
	    p->extra.x = 0;
	}
    }

    return true;
}

void BaseEnemy::updateOffset( void)
{
//    XTRACE();
    //update offset to end of path
    Point3D p1;
    _activePath->end( p1);
    _offset = _offset + p1;
}

void BaseEnemy::sendHome( void)
{
//    XTRACE();
    _moveHome = false;
    
    _nextMoveState = eIdle;
    _activePathIndex = 0;
    _nextPath = _lEnemy->idle;
    GameState::enemyAttackCount--;

    //transition back to home from staging area
    _offset.x += (Random::rangef0_1() * 30.0f) - 15.0f;
    _offset.y = 60.0;

    //since we transported to the staging area, we need an extra update
    _extraUpdateAfterTransport = true;

    Point3D p1 = Point3D( 0,0,0);
    Point3D p2;

    _transitionBC.SetControlPoint( 0, p1);

    p2 = p1 + Point3D( 0, -10, 0);
    _transitionBC.SetControlPoint( 1, p2);

    p1 = _lEnemy->home - _offset;
    _transitionBC.SetControlPoint( 3, p1);

    p2 = p1 + Point3D( 0, 10, 0);
    _transitionBC.SetControlPoint( 2, p2);

    _transitionPath.update();
    _moveState = eTransition;
    _activePath = &_transitionPath;
}

void BaseEnemy::getNextPath( void)
{
//    XTRACE();
    updateOffset();

    if( _moveHome)
    {
        sendHome();
        return;
    }
    
    switch( _moveState)
    {
	case eIdle:
	    if( ( GameState::enemyAttackCount < Skill::getMaxAttacking()) &&
		( _lEnemy->numAttackPaths > 0) &&
                (( Random::random() & 0xff) > 0x80))
	    {
		_moveState = eAttack;
		_activePathIndex = Random::random() % _lEnemy->numAttackPaths;
		_activePath = _lEnemy->attack[ _activePathIndex];
		AudioS::instance()->playSample( "sounds/attackWave.wav");
		GameState::enemyAttackCount++;
	    }
	    break;

	case eEntry:
	    {
		_nextMoveState = eIdle;
		_nextPath = _lEnemy->idle;

		Point3D p1 = Point3D( 0,0,0);
		Point3D p2;

		_transitionBC.SetControlPoint( 0, p1);
//LOG_INFO << "p1 = " << p1.x << "," << p1.y << endl;
//LOG_INFO << "off = " << _offset.x << "," << _offset.y << endl;

		_activePath->endT(p2);
		p2 = p1 - p2;
		_transitionBC.SetControlPoint( 1, p2);

		p1 = _lEnemy->home - _offset;
		_transitionBC.SetControlPoint( 3, p1);

		p2 = p1 + Point3D( 0, 50, 0);
		_transitionBC.SetControlPoint( 2, p2);

		_transitionPath.update();
		_moveState = eTransition;
		_activePath = &_transitionPath;
	    }
	    break;

	case eAttack:
	    if( (_lEnemy->numRetreatPaths > 0) &&
		( (Random::random() & 0xff) < 255))
	    {
		//every now end then retreat
		_moveState = eRetreat;
		_activePathIndex = Random::random() % _lEnemy->numRetreatPaths;
		_activePath = _lEnemy->retreat[ _activePathIndex];
	    }

	    if( _offset.y < -60.0)
	    {
		sendHome();
	    }
	    break;

	case eRetreat:
	    //enough retreat, go back to attack
	    if( _lEnemy->numAttackPaths > 0)
	    {
		_moveState = eAttack;
		_activePathIndex = Random::random() % _lEnemy->numAttackPaths;
		_activePath = _lEnemy->attack[ _activePathIndex];
	    }

	    if( _offset.y > 60.0)
	    {
		sendHome();
	    }
	    break;

	case eTransition:
//                _offset = _lEnemy->home;
	    _moveState = _nextMoveState;
	    _activePath = _nextPath;
	    break;

	default:
	    _activePath = _lEnemy->idle;
	    break;
    }
}

void BaseEnemy::hit( ParticleInfo *p, int damage, int /*radIndex*/)
{ 
//    XTRACE();
    static ParticleGroup *effects =
	ParticleGroupManagerS::instance()->getParticleGroup( EFFECTS_GROUP2);

    _energy -= damage;
    if( _energy <= 0)
    {
	static ParticleGroup *bonus =
	    ParticleGroupManagerS::instance()->getParticleGroup( BONUS_GROUP);

	p->tod = 0; 
	if( _moveState == eAttack)
	{
	    ScoreKeeperS::instance()->addToCurrentScore( 200);
	}
	else
	{
	    ScoreKeeperS::instance()->addToCurrentScore( 100);
	}

	if( (Random::random() & 0xff) > 10)
	    bonus->newParticle( "Bonus1", 
		p->position.x, p->position.y, p->position.z);
	else
	    bonus->newParticle( "SuperBonus", 
		p->position.x, p->position.y, p->position.z);

	//spawn explosion
	for( int i=0; i<(int)(GameState::horsePower/2.0); i++)
	{
	    effects->newParticle(
		"ExplosionPiece", p->position.x, p->position.y, p->position.z);
	}
	AudioS::instance()->playSample( "sounds/explosion.wav");
    }
    else
    {
	//a hit but no kill
	ScoreKeeperS::instance()->addToCurrentScore( 50);

	ParticleInfo pi;

	pi.position.x = p->position.x;
	pi.position.y = p->position.y;
	pi.position.z = p->position.z;

	pi.velocity.x = _dir.x*0.6f*GAME_STEP_SCALE;
	pi.velocity.y = _dir.y*0.6f*GAME_STEP_SCALE;
	pi.velocity.z = _dir.z*0.6f*GAME_STEP_SCALE;

	effects->newParticle( "MiniSmoke", pi);
    }
}

//transforms y axis to 'at' vector
void BaseEnemy::alignWith( Point3D &dohAt)
{
//    XTRACE();
    GLfloat m[16];

    //All our model point towards -y, doh! Flip them around...
    Point3D at = dohAt * -1.0f;
    norm( at);

    Point3D z( 0.0, 0.0, 1.0);

    Point3D x = cross( at, z);
    norm( x);

    z = cross( x, at);
    norm( z);

#define M(row,col)  m[col*4+row]
    M(0, 0) =  x.x; M(1, 0) =  x.y; M(2, 0) =  x.z; M(3, 0) = 0.0;
    M(0, 1) = at.x; M(1, 1) = at.y; M(2, 1) = at.z; M(3, 1) = 0.0;
    M(0, 2) =  z.x; M(1, 2) =  z.y; M(2, 2) =  z.z; M(3, 2) = 0.0;
    M(0, 3) =  0.0; M(1, 3) =  0.0; M(2, 3) =  0.0; M(3, 3) = 1.0;

    glMultMatrixf(m);
}
