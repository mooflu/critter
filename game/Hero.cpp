// Description:
//   Our Hero!
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

#include <Hero.hpp>
#include <ParticleGroup.hpp>
#include <ParticleGroupManager.hpp>
#include <WeaponDepot.hpp>
#include <Weapon.hpp>
#include <Timer.hpp>
#include <Trace.hpp>
#include <GameState.hpp>
#include <Constants.hpp>
#include <ModelManager.hpp>
#include <Audio.hpp>
#include <Point.hpp>
#include <ScoreKeeper.hpp>
#include <Config.hpp>
#include <Game.hpp>
#include <Input.hpp>
#include <Random.hpp>

#include <BitmapManager.hpp>

#include "GLee.h"

const float MAX_X= 63; //(int)(47.5*4/3);
const float MIN_X=-63; //-(int)(47.5*4/3);
const float MAX_Y= 45;
const float MIN_Y=-45;

Hero::Hero():
    ParticleType( "Hero"),
    pInfo(0),
    _maxY(MIN_Y),
    _maxWeaponEnergy(100.0)
{
    XTRACE();
    for( int i=0; i<360; i++)
    {
	_sint[i] = sin( i * ((float)M_PI/180.0f));
	_cost[i] = cos( i * ((float)M_PI/180.0f));
    }
    
    WeaponDepotS::instance(); //pre-load weapon models
    
    reset();
}

void Hero::reset( void)
{
    XTRACE();
    _isAlive = true;
    _isDying = false;
    _isDyingDelay = 0;
    _moveLeft=0;
    _moveRight=0;
    _moveUp=0;
    _moveDown=0;
    _energy=100;
    _shieldEnergy=30;
    _weaponEnergy=_maxWeaponEnergy;
    _damageMultiplier = 1.0;
    _weaponPowerup = 0;
    _bonusStreak = 0;

    for( int i=0; i<Hero::MAX_WEAPONS; i++)
    {
        _weapon[ i] = 0;
        _weaponAutofire[ i] = false;
        _weaponReload[ i] = 0.0;
#define UNLIMITED_AMMO -1
        _weaponAmmo[ i] = UNLIMITED_AMMO;
    }
}

Hero::~Hero()
{
    XTRACE();
}

void Hero::init( ParticleInfo *p)
{
    XTRACE();
    pInfo = p;

    //override Y
    p->position.y = MIN_Y;

    vec3 minpt, maxpt;
    _model->getBoundingBox( minpt, maxpt);
    p->radius = (maxpt.x-minpt.x)/2.0;
    p->damage = 500;

    updatePrevs(p);
}

void Hero::assignWeapons( Skill::SkillEnum skill)
{
    WeaponDepot *wDepot = WeaponDepotS::instance();
    switch( skill)
    {
	case Skill::eInsane:
	    _weapon[ Hero::PRIMARY_WEAPON] = 
	        wDepot->getWeapon( WeaponWINGPHASER);
	    _weapon[ Hero::SECONDARY_WEAPON] = 
	        wDepot->getWeapon( WeaponFLANKBURST);
	    _weapon[ Hero::TERTIARY_WEAPON] = 
	        wDepot->getWeapon( WeaponICESPRAY);
	    break;

	case Skill::eExpert:
	    _weapon[ Hero::PRIMARY_WEAPON] = 
	        wDepot->getWeapon( WeaponSTINGER);
	    _weapon[ Hero::SECONDARY_WEAPON] = 
	        wDepot->getWeapon( WeaponFLANKBURST);
	    _weapon[ Hero::TERTIARY_WEAPON] = 
	        wDepot->getWeapon( WeaponICESPRAY);
	    break;

	default:
	    _weapon[ Hero::PRIMARY_WEAPON] = 
	        wDepot->getWeapon( WeaponSTINGER);
	    _weapon[ Hero::SECONDARY_WEAPON] = 
	        wDepot->getWeapon( WeaponFLANKBURST);
	    _weapon[ Hero::TERTIARY_WEAPON] = 
	        wDepot->getWeapon( WeaponDOG);
	    break;
    }
}

void Hero::addEnergy( int val)
{
    _energy += val;
    if( _energy > 100)
    {
	//add unused energy as score
	ScoreKeeperS::instance()->addToCurrentScore( (_energy-100)*10);
	_energy = 100;
    }
}

void Hero::addShield( int val)
{
    _shieldEnergy += val;
    if( _shieldEnergy > 100)
    {
	//add unused energy as score
	ScoreKeeperS::instance()->addToCurrentScore( (_shieldEnergy-100)*10);
	_shieldEnergy = 100;
    }
}

void Hero::hit( ParticleInfo * p, ParticleInfo *p2, int /*radIndex*/)
{
//    XTRACE();
    if( !_isAlive || _isDying) return;
    
    
    int damage = p2->damage;
    //cout << "Hero hit with " << damage << endl;
    
    if( _shieldEnergy > 0)
    {
        _shieldEnergy -= damage;
        if( _shieldEnergy < 0)
        {
            _energy += _shieldEnergy;
            _shieldEnergy = 0;
        }
    }
    else
    {
	_energy -= damage;
    }

    if( damage > 0)
    {
	//as an extra penalty, reset any armor piercing...
	setArmorPierce( 1.0f);

	AudioS::instance()->playSample( "sounds/bang.wav");
    }

    //hero dead...
    if( _energy <= 0)
    {
	static ParticleGroup *effects =
	    ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);

	AudioS::instance()->playSample( "sounds/big_explosion.wav");

	//spawn explosion
	for( int i=0; i<(int)(GameState::horsePower/1.5); i++)
	{
	    effects->newParticle(
		"ExplosionPiece", p->position.x, p->position.y, p->position.z);
	}
        _isDyingDelay = 60; //2 sec
        _isDying = true;
        p->damage = 0;
    }
}

bool Hero::init( void)
{
    XTRACE();
    _model = ModelManagerS::instance()->getModel("models/Hero");
    return(_model!=0);
}

void Hero::spawnSparks( int spawnCount, float r, ParticleInfo &pi)
{
    static ParticleGroup *effects =
	ParticleGroupManagerS::instance()->getParticleGroup( EFFECTS_GROUP3);

    for( int i=0; i<spawnCount; i++)
    {
	int sparkType = Random::random()%3;
	string effect;
	switch( sparkType)
	{
	    default:
	    case 0:
	        effect = "FireSpark1";
		break;
	    case 1:
	        effect = "FireSpark2";
		break;
	    case 2:
	        effect = "FireSpark3";
		break;
	}

	int rot = Random::random()%360;
	float sina = _sint[rot] * r;
	float cosa = _cost[rot] * r;

	effects->newParticle( effect, pi.position.x+sina, pi.position.y+cosa, pi.position.z);
    }
}


void Hero::allowVerticalMovement(bool allow)
{
    if( allow)
	_maxY = MAX_Y;
    else
	_maxY = MIN_Y;
}

bool Hero::update( ParticleInfo *p)
{
//    XTRACE();
    if( _isDying)
    {
        if( _isDyingDelay-- < 0)
        {
            _isAlive = false;
            return false;
        }
        return true;
    }
    
    if( !_isAlive) return false;

    updatePrevs(p);

    float & _xPos = p->position.x;
    float & _yPos = p->position.y;

    _weaponEnergy += 3.0f * GAME_STEP_SCALE;

    Clamp( _weaponEnergy, 0.0, _maxWeaponEnergy);

    if( _moveLeft)
    {
	_xPos += _moveLeft; 
        Clamp( _xPos, MIN_X, MAX_X);
    }
    if( _moveRight)
    {
	_xPos += _moveRight; 
        Clamp( _xPos, MIN_X, MAX_X);
    }

    if( _moveUp)
    {
	_yPos += _moveUp; 
        Clamp( _yPos, MIN_Y, _maxY);
    }
    if( _moveDown)
    {
	_yPos += _moveDown; 
        Clamp( _yPos, MIN_Y, _maxY);
    }

    for( int i=0; i<Hero::MAX_WEAPONS; i++)
    {
	if( _weaponAutofire[ i] && weaponLoaded( i)) 
	{
	    weaponFire( true, i);
	}
    }

    spawnSparks( _shieldEnergy/3, pInfo->radius, *p);

    lastXPos = _xPos;
    lastYPos = _yPos;

    return true;
}

bool Hero::weaponLoaded( int weapNum)
{
//    XTRACE();
    if( _weaponAmmo[ weapNum] == 0) return false;
    return( _weaponReload[ weapNum] < GameState::stopwatch.getTime());
}

void Hero::move( float dx, float dy)
{
//    XTRACE();
    if( !_isAlive) return;
    if( !pInfo) return;

    float & _xPos = pInfo->position.x;
    float & _yPos = pInfo->position.y;

    _xPos += dx;
    _yPos += dy;

    Clamp( _xPos, MIN_X, MAX_X);
    Clamp( _yPos, MIN_Y, _maxY);
}

void Hero::move( Direction::DirectionEnum dir, bool isDown)
{
//    XTRACE();
    if( !_isAlive) return;
    float delta;

    if( isDown) 
    {
	delta = 3.0f * GAME_STEP_SCALE;
    }
    else
    {
	delta = 0;
    }

//    LOG_INFO << "delta = " << delta << endl;

    switch( dir)
    {
        case Direction::eDown:
            _moveDown = -delta;
            break;
        case Direction::eUp:
            _moveUp = delta;
            break;

        case Direction::eLeft:
            _moveLeft = -delta;
            break;
        case Direction::eRight:
            _moveRight = delta;
            break;

        default:
            break;
    }
}

void Hero::weaponFire( bool isDown, int weapNum)
{
//    XTRACE();
    if( !_isAlive) 
    {
	if( weapNum == 2)
	{
	    GameS::instance()->startNewGame();
	}
	return;
    }
    if( !pInfo) return;

    float & _xPos = pInfo->position.x;
    float & _yPos = pInfo->position.y;
    float & _zPos = pInfo->position.z;

    static bool needButtonUp = false;
    if( isDown == false)
    {
        needButtonUp = false;
    }
    if( needButtonUp)
    {
        return;
    }

    _weaponAutofire[ weapNum] = isDown;
    if( isDown && weaponLoaded( weapNum))
    {
        if( !_weapon[ weapNum])
        {
            LOG_WARNING << "No weapon mounted in slot " << weapNum << endl;
            return;
        }

        float er = _weapon[ weapNum]->energyRequired();
        if( _weaponEnergy < er)
        {
            return;
        }

	ConfigS::instance()->getBoolean( "autofireOn", _autofireOn);
	if( ! _autofireOn)
	{
	    needButtonUp = true;
	}
        _weaponEnergy -= er;

        //FIXME: offset depending on where weapon is mounted
        _weapon[ weapNum]->launch( _xPos, _yPos, _zPos);

        _weaponReload[ weapNum] = _weapon[ weapNum]->reloadTime() +
            GameState::stopwatch.getTime(); 
        if( _weaponAmmo[ weapNum] > 0)
        {
	    _weaponAmmo[ weapNum]--;
        }
    }
}

void Hero::setArmorPierce( float damageMultiplier)
{
    _damageMultiplier = damageMultiplier;
    for( int i=0; i<Hero::MAX_WEAPONS; i++)
    {
	_weapon[ i]->setDamageMultiplier( _damageMultiplier);
    }
}

void Hero::drawWeapon( unsigned int weapNum)
{
    if( (weapNum < Hero::MAX_WEAPONS) &&  _weapon[ weapNum]) _weapon[weapNum]->draw();
}

void Hero::draw( void)
{
//    XTRACE();
    if( _isDying) return;
    if( !_isAlive) return;
    if( !pInfo) return;

    ParticleInfo pi;
    interpolate( pInfo, pi);

    glDisable( GL_DEPTH_TEST);
    glDisable( GL_LIGHTING);

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //draw a halo around hero...
    GLBitmapCollection *bitmaps = 
	BitmapManagerS::instance()->getBitmap( "bitmaps/ammo");
    static bool wasInit = false;
    static int shield;
    if( !wasInit)
    {
	shield = bitmaps->getIndex( "Shield");
	wasInit = true;
    }

    glEnable(GL_TEXTURE_2D);
    bitmaps->bind();
    float r = pInfo->radius/((42.0f-2.0f)/2.0f);
    glColor4f(1.0,1.0,1.0,(float)_shieldEnergy/(float)200.0);
    bitmaps->DrawC( shield, 0, 0, r, r);
    glDisable(GL_TEXTURE_2D);

    glEnable( GL_LIGHTING);
    glEnable( GL_DEPTH_TEST);

    _model->draw();
}
