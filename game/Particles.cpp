// Description:
//   All kinds of particles.
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
#include "Particles.hpp"

#include <Trace.hpp>
#include <Constants.hpp>
#include <GameState.hpp>
#include <Random.hpp>
#include <ModelManager.hpp>
#include <Camera.hpp>
#include <ParticleGroup.hpp>
#include <ParticleGroupManager.hpp>
#include <Hero.hpp>
#include <Audio.hpp>
#include <FontManager.hpp>
#include <BitmapManager.hpp>
#include <ScoreKeeper.hpp>
using namespace std;
//------------------------------------------------------------------------------
void Particles::Initialize( void)
{
    static bool initialized = false;
    if( !initialized)
    {
	new HeroStinger();
	new StingerTrail();
	new SmokePuff();
	new MiniSmoke();
	new ExplosionPiece();
	new PlasmaBullet();
	new Bonus1( "SuperBonus", 1000);
	new Bonus1( "Bonus1", 100);
	new ScoreHighlight();
	new EnergyBlob();
	new ShieldBoost();
	new ArmorPierce();
	new WeaponUpgrade();
	new StatusMessage();
	new Spark();
	new BallOfFire();
	new BallOfIce();
	new BallOfPoison();
	new SwarmLeader();
	new SwarmElement();
	new Phaser();
	new FireSpark( "FireSpark1");
	new FireSpark( "FireSpark2");
	new FireSpark( "FireSpark3");
	new FlankBurst( "FlankBurstLeft");
	new FlankBurst( "FlankBurstRight");
	HeroS::instance();

	initialized = true;
    }
}

float getPseudoRadius( Model *model)
{
//if most of the movement is vertical, width represents the radius
//most realistically (for non square objs).
    vec3 minpt, maxpt;
    model->getBoundingBox( minpt, maxpt);
    return (maxpt.x-minpt.x)/2.0;
}

//------------------------------------------------------------------------------

GLBitmapCollection *BitmapParticleType::_bitmaps = 0;

BitmapParticleType::BitmapParticleType( const string &particleName):
    ParticleType( particleName)
{
    XTRACE();
    LoadBitmaps();
}

BitmapParticleType::~BitmapParticleType()
{
    XTRACE();
}

void BitmapParticleType::LoadBitmaps( void)
{
    XTRACE();
    static bool bitmapsLoaded = false;
    if( !bitmapsLoaded)
    {
	_bitmaps = BitmapManagerS::instance()->getBitmap( "bitmaps/ammo");
	if( !_bitmaps)
	{
	    LOG_ERROR << "Unable to load particle bitmap" << endl;
	}
        bitmapsLoaded = true;
    }
}

//------------------------------------------------------------------------------

SingleBitmapParticle::SingleBitmapParticle( 
    const string &particleName, const char *bitmapName) 
    :BitmapParticleType(particleName)
{
    XTRACE();
    string bmName( bitmapName);
    _bmIndex = _bitmaps->getIndex( bmName);

    _bmHalfWidth = (float)(_bitmaps->getWidth( _bmIndex))/2.0f;
    _bmHalfHeight= (float)(_bitmaps->getHeight( _bmIndex))/2.0f;

//if most of the movement is vertical, width represents the radius
//most realisticly (for non square objs).
//        _radius = sqrt(_bmHalfWidth*_bmHalfWidth + _bmHalfHeight*_bmHalfHeight);
    _radius = _bmHalfWidth;// + _bmHalfHeight)/2.0;
}

//------------------------------------------------------------------------------

SmokePuff::SmokePuff( void):SingleBitmapParticle( "SmokePuff", "SmokePuff1")
{
    XTRACE();
}

SmokePuff::~SmokePuff()
{
    XTRACE();
}

void SmokePuff::init( ParticleInfo *p)
{
    XTRACE();
    p->velocity.x = 0.0f * GAME_STEP_SCALE;
    p->velocity.y = 0.27f* GAME_STEP_SCALE;
    p->velocity.z = 0.0f * GAME_STEP_SCALE;

    p->extra.x = 0.025f * GAME_STEP_SCALE;
    p->extra.y = 0.004f * GAME_STEP_SCALE;
    p->extra.z = 0.36f;

    //init previous values for interpolation
    updatePrevs(p);
}

bool SmokePuff::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

    p->extra.z = 0.36f - p->extra.x*6.0f;
    //if alpha reaches 0, we can die
    if( p->extra.z < 0) return false;

    p->extra.x    += p->extra.y;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    return true;
}

void SmokePuff::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    float shiftX = _bmHalfWidth*pi.extra.x;
    float shiftY = _bmHalfHeight*pi.extra.x;
    glTranslatef( pi.position.x-shiftX, pi.position.y-shiftY, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

//	LOG_ERROR << pi.extra.x << endl;
    glColor4f(1.0,1.0,1.0, pi.extra.z);

    bindTexture();
    _bitmaps->Draw( _bmIndex, 0, 0, pi.extra.x, pi.extra.x);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

MiniSmoke::MiniSmoke( void):SingleBitmapParticle( "MiniSmoke", "SmokePuff1")
{
    XTRACE();
}

MiniSmoke::~MiniSmoke()
{
    XTRACE();
}

void MiniSmoke::init( ParticleInfo *p)
{
    XTRACE();

    p->extra.x = 0.020f * GAME_STEP_SCALE;
    p->extra.y = 0.002f * GAME_STEP_SCALE;
    p->extra.z = 0.80f;

    //init previous values for interpolation
    updatePrevs(p);
}

bool MiniSmoke::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

    p->extra.z = 0.80f - p->extra.x*12.0f;
    //if alpha reaches 0, we can die
    if( p->extra.z < 0) return false;

    p->extra.x    += p->extra.y;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    return true;
}

void MiniSmoke::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    float shiftX = _bmHalfWidth*pi.extra.x;
    float shiftY = _bmHalfHeight*pi.extra.x;
    glTranslatef( pi.position.x-shiftX, pi.position.y-shiftY, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

//	LOG_ERROR << pi.extra.x << endl;
    glColor4f(1.0,1.0,1.0, pi.extra.z);

    bindTexture();
    _bitmaps->Draw( _bmIndex, 0, 0, pi.extra.x, pi.extra.x);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

Spark::Spark( void):SingleBitmapParticle( "Spark", "Spark1")
{
    XTRACE();
}

Spark::~Spark()
{
    XTRACE();
}

void Spark::init( ParticleInfo *p)
{
    XTRACE();
    p->velocity.x =  (float)(Random::random()&0xf) * 0.82f * GAME_STEP_SCALE;
    p->velocity.y = -(float)(Random::random()&0xf) * 0.62f * GAME_STEP_SCALE;
    p->velocity.z =  0.0f  * GAME_STEP_SCALE;

    p->extra.x = 0.10f * GAME_STEP_SCALE;
//    p->extra.z = 0.36;

    //init previous values for interpolation
    updatePrevs(p);
}

bool Spark::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

    p->velocity.y -= 0.8f * GAME_STEP_SCALE;

    p->extra.z = 0.9f - p->extra.x;
    //if alpha reaches 0, we can die
    if( p->extra.z < 0) return false;

    p->extra.x += 0.025f * GAME_STEP_SCALE;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    if( p->position.y < 0) return false;

    return true;
}

void Spark::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolateOther( p, pi);

    glColor4f(1.0,1.0,1.0, pi.extra.z);

    bindTexture();
    _bitmaps->DrawC( _bmIndex, pi.position.x, pi.position.y, 1.0, 1.0);
}

//------------------------------------------------------------------------------

FireSpark::FireSpark( const string &sp):SingleBitmapParticle( sp, sp.c_str())
{
    XTRACE();
}

FireSpark::~FireSpark()
{
    XTRACE();
}

void FireSpark::init( ParticleInfo *p)
{
    XTRACE();
    p->velocity.x = (float)((Random::random()&0xff)-0x80) * 0.0010f * GAME_STEP_SCALE;
    p->velocity.y = (float)((Random::random()&0xff)-0x80) * 0.0010f * GAME_STEP_SCALE;
    p->velocity.z =  0.0f  * GAME_STEP_SCALE;

    p->extra.x = 0.10f * GAME_STEP_SCALE;
//    p->extra.z = 0.36;

    //init previous values for interpolation
    updatePrevs(p);
}

bool FireSpark::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

//    p->velocity.y -= 0.8 * GAME_STEP_SCALE;

    p->extra.z = 0.9f - p->extra.x;
    //if alpha reaches 0, we can die
    if( p->extra.z < 0) return false;

    p->extra.x += 0.10f * GAME_STEP_SCALE;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    return true;
}

void FireSpark::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

    glColor4f(1.0,1.0,1.0, p->extra.z);

    bindTexture();

//    glRotatef( pi.extra.y, 0,0,1);

    _bitmaps->DrawC( _bmIndex, 0, 0, 0.2f, 0.2f);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

FlankBurst::FlankBurst( const string &sp):
  SingleBitmapParticle( sp, sp.c_str()),
  _dir(1.0)
{
    XTRACE();
    if( sp == "FlankBurstLeft")
    {
	_dir = -1.0;
    }
}

FlankBurst::~FlankBurst()
{
    XTRACE();
}

void FlankBurst::init( ParticleInfo *p)
{
    XTRACE();
    p->velocity.x = 2.0f * GAME_STEP_SCALE * _dir;
    p->velocity.y = 0.0f * GAME_STEP_SCALE;
    p->velocity.z = 0.0f * GAME_STEP_SCALE;

    p->damage = 500;
    p->tod = -1;

    p->extra.x = 0.008f * GAME_STEP_SCALE;
    p->extra.y = 0.02f;
    p->radius = _bmHalfWidth*p->extra.y*2.0f;

    //init previous values for interpolation
    updatePrevs(p);
}

bool FlankBurst::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    p->extra.y += p->extra.x;
    p->radius = _bmHalfWidth*p->extra.y*2.0f;

    if( (fabs( p->position.x) > 68.0f)) 
    {
	return false;
    }

    return true;
}

void FlankBurst::hit( ParticleInfo * /*p*/, int /*damage*/, int /*radIndex*/) 
{ 
    /*
    p->tod = 0; 
    */
    AudioS::instance()->playSample( "sounds/whoop.wav");
}

void FlankBurst::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

    glColor4f(1.0,1.0,1.0, 1.0);

    bindTexture();
    _bitmaps->DrawC( _bmIndex, 0, 0, pi.extra.y, pi.extra.y);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

BallOfPoison::BallOfPoison( void): 
    SingleBitmapParticle( "BallOfPoison", "BallOfPoison")
{
    XTRACE();
}

BallOfPoison::~BallOfPoison()
{
    XTRACE();
}

void BallOfPoison::init( ParticleInfo *p)
{
    XTRACE();
    /*
    p->velocity.x = 2.0 * GAME_STEP_SCALE;
    p->velocity.y = 0.0 * GAME_STEP_SCALE;
    p->velocity.z = 0.0 * GAME_STEP_SCALE;
    */
//    p->damage = 50;

    p->radius = _bmHalfWidth*0.05f;
    p->tod = -1;

    //init previous values for interpolation
    updatePrevs(p);
}

bool BallOfPoison::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    if( (fabs( p->position.x) > 68.0)) 
    {
	return false;
    }

    if( (fabs( p->position.y) > 50.0)) 
    {
	return false;
    }

    return true;
}

void BallOfPoison::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{ 
    p->tod = 0; 
    AudioS::instance()->playSample( "sounds/whoop.wav");
}

void BallOfPoison::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

    glColor4f(1.0,1.0,1.0, 1.0);

    bindTexture();
    _bitmaps->DrawC( _bmIndex, 0, 0, 0.2f, 0.2f);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

BallOfIce::BallOfIce( void): SingleBitmapParticle( "BallOfIce", "BallOfIce")
{
    XTRACE();
}

BallOfIce::~BallOfIce()
{
    XTRACE();
}

void BallOfIce::init( ParticleInfo *p)
{
    XTRACE();
    /*
    p->velocity.x = 2.0 * GAME_STEP_SCALE;
    p->velocity.y = 0.0 * GAME_STEP_SCALE;
    p->velocity.z = 0.0 * GAME_STEP_SCALE;
    */
//    p->damage = 50;

    p->radius = _bmHalfWidth*0.05f;
    p->tod = -1;

    //init previous values for interpolation
    updatePrevs(p);
}

bool BallOfIce::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    if( (fabs( p->position.x) > 68.0)) 
    {
	return false;
    }

    if( (fabs( p->position.y) > 50.0)) 
    {
	return false;
    }

    return true;
}

void BallOfIce::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{ 
    p->tod = 0; 
    AudioS::instance()->playSample( "sounds/whoop.wav");
}

void BallOfIce::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

    glColor4f(1.0,1.0,1.0, 1.0);

    bindTexture();
    _bitmaps->DrawC( _bmIndex, 0, 0, 0.2f, 0.2f);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}


//------------------------------------------------------------------------------

SwarmLeader::SwarmLeader( void): SingleBitmapParticle( "SwarmLeader", "BallOfIce"),
    _toHero(64)
{
    XTRACE();
    _toHero.AddSegment( Point3D(0,0,0));
    _toHero.AddSegment( Point3D(0,0,0));
}

SwarmLeader::~SwarmLeader()
{
    XTRACE();
}

void SwarmLeader::init( ParticleInfo *p)
{
    XTRACE();

    p->radius = _bmHalfWidth*0.5f;
    p->tod = -1;

    p->damage = 0;
    p->extra.x = 0.0f; //dt 0..1
    p->extra.y = 0.0f; //# of attached swarm elements

    p->points[0] = Point3D( p->position.x, p->position.y, 0); 
    p->points[1] = p->points[0] + Point3D( p->velocity.x*30, p->velocity.y*30, 0);
    p->points[3] = Point3D( HeroS::instance()->lastXPos, HeroS::instance()->lastYPos, 0); 
    p->points[2] = p->points[3] + Point3D( p->velocity.x*30, -p->velocity.y*30, 0);

    GameState::enemyBulletCount++;

    //init previous values for interpolation
    updatePrevs(p);
}

bool SwarmLeader::update( ParticleInfo *p)
{
//    XTRACE();
    if( (p->extra.y<0.5) && ((fabs(p->position.x) > 70.0) || (fabs(p->position.y) > 50.0)))
    {
	GameState::enemyBulletCount--;
	return false;
    }

    //update previous values for interpolation
    updatePrevs(p);

    _toHero.SetControlPoint( 0, p->points[0]);
    _toHero.SetControlPoint( 1, p->points[1]);
    _toHero.SetControlPoint( 2, p->points[2]);
    _toHero.SetControlPoint( 3, p->points[3]);
    if( p->extra.x <= 1.0f)
    {
	Point3D pos;
	_toHero.GetPos( p->extra.x, pos);
	p->extra.x += 0.01 * GAME_STEP_SCALE;
	p->position.x = pos.x;
	p->position.y = pos.y;
    }
    else
    {
	Point3D pos1,pos2;
	_toHero.GetPos( 1.0, pos1);
	_toHero.GetPos( 1.0-(0.01 * GAME_STEP_SCALE), pos2);
	pos1 = pos1 - pos2;
	p->position.x += pos1.x;
	p->position.y += pos1.y;
    }

    return true;
}

void SwarmLeader::draw( ParticleInfo *)
{
//    XTRACE();
#if 0
    ParticleInfo pi;
    interpolate( p, pi);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    Point3D v(p->velocity.x, p->velocity.y, 0);
    v = v * 0.5;
    float d = dist(v);
    if( d>1.0)
	v = v * (1.0/d);
    if( d<0.5)
	v = v * (0.5/d);

    Point3D vp = Point3D(-v.y, v.x, 0);

    Point3D p1 = v*2;
    Point3D p2 = vp;
    Point3D p3 = v * -1.0;
    Point3D p4 = vp * -1.0;

    glBegin(GL_QUADS);
    glColor4f( 1.0, 0.0, 0.2, 0.8);
    glVertex3f( p1.x, p1.y, 0);
    glColor4f( 1.0, 0.0, 0.2, 0.5);
    glVertex3f( p2.x, p2.y, 0);
    glColor4f( 1.0, 0.0, 0.2, 0.1);
    glVertex3f( p3.x, p3.y, 0);
    glColor4f( 1.0, 0.0, 0.2, 0.5);
    glVertex3f( p4.x, p4.y, 0);
    glEnd();

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
//#else
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

    glColor4f(1.0,1.0,1.0, 1.0);

    bindTexture();
    _bitmaps->DrawC( _bmIndex, 0, 0, 0.2f, 0.2f);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
#endif
}

//------------------------------------------------------------------------------

SwarmElement::SwarmElement( void):ParticleType( "SwarmElement")
{
    XTRACE();
}

SwarmElement::~SwarmElement()
{
    XTRACE();
}

void SwarmElement::init( ParticleInfo *p)
{
    XTRACE();

    p->radius = 1.0;
    p->tod = -1;
    p->damage = 1;

    p->related->extra.y++; 

    //init previous values for interpolation
    updatePrevs(p);
}

void SwarmElement::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{ 
    p->tod = 0;
//    AudioS::instance()->playSample( "sounds/whoop.wav");
}

bool SwarmElement::update( ParticleInfo *p)
{
//    XTRACE();

    if( (p->tod==0) || (fabs( p->position.x) > 70.0) || (fabs( p->position.y) > 50.0) )
    {
	p->related->extra.y--; 
	return false;
    }

    vec3 a = p->related->position - p->position;
    Point3D a2( a.x+(Random::rangef0_1()-0.5), a.y+(Random::rangef0_1()-0.5), 0);
    if( dist(a2) > 0.0001)
    {
	norm(a2);
	a2 = a2 * 0.2;
	Point3D v(p->velocity.x, p->velocity.y, 0);
	v = v+a2;
	float d = dist(v);
	if( d > 2.0)
	{
	    v = v*(2.0/d);
	}
	p->velocity.x = v.x;
	p->velocity.y = v.y;
    }

    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    return true;
}

void SwarmElement::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    Point3D v(p->velocity.x, p->velocity.y, 0);
    v = v * 0.5;
    float d = dist(v);
    if( d>1.0)
	v = v * (1.0/d);
    if( d<0.5)
	v = v * (0.5/d);

    Point3D vp = Point3D(-v.y, v.x, 0);

    Point3D p1 = v*2;
    Point3D p2 = vp;
    Point3D p3 = v * -1.0;
    Point3D p4 = vp * -1.0;

    GLfloat swarmElementVertices[] = {
        p1.x, p1.y, 0,
        p2.x, p2.y, 0,
        p3.x, p3.y, 0,
        p4.x, p4.y, 0
    };
    
    GLfloat swarmElementColors[] = {
        1.0, 1.0, 0.2, 0.8,
        1.0, 1.0, 0.2, 0.5,
        1.0, 1.0, 0.2, 0.1,
        1.0, 1.0, 0.2, 0.5
    };
    
    glVertexPointer(3, GL_FLOAT, 0, swarmElementVertices);
    glColorPointer(4, GL_FLOAT, 0, swarmElementColors);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------------

Phaser::Phaser( void): SingleBitmapParticle( "WingPhaser1", "WingPhaser1")
{
    XTRACE();
}

Phaser::~Phaser()
{
    XTRACE();
}

void Phaser::init( ParticleInfo *p)
{
    XTRACE();

    p->velocity.x = 0.0f * GAME_STEP_SCALE;
    p->velocity.y = 4.0f * GAME_STEP_SCALE;
    p->velocity.z = 0.0f * GAME_STEP_SCALE;

    p->radius = _bmHalfWidth*0.05f;
//    p->damage = 50;
    p->tod = -1;

    //init previous values for interpolation
    updatePrevs(p);
}

bool Phaser::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    if( (fabs( p->position.x) > 68.0f)) 
    {
	return false;
    }

    if( (fabs( p->position.y) > 50.0f)) 
    {
	return false;
    }

    return true;
}

void Phaser::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{ 
    p->tod = 0; 
    AudioS::instance()->playSample( "sounds/whoop.wav");
}

void Phaser::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

//    glColor4f(1.0,1.0,1.0, 1.0);
    glColor4f( p->color.x, p->color.y, p->color.z, 1.0);

    bindTexture();
    _bitmaps->DrawC( _bmIndex, 0, 0, 0.2f, 0.2f);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

PlasmaBullet::PlasmaBullet( void):BitmapParticleType( "PlasmaBullet")
{
    XTRACE();
    string plasmaBullet;
    plasmaBullet = "PlasmaBullet1";
    _bulletIndex0 = _bitmaps->getIndex( plasmaBullet);
    plasmaBullet = "PlasmaBullet2";
    _bulletIndex1 = _bitmaps->getIndex( plasmaBullet);

    //assume both smoke puffs are the same size
    _bmHalfWidth = _bitmaps->getWidth( _bulletIndex0)/2.0f;
    _bmHalfHeight= _bitmaps->getHeight( _bulletIndex0)/2.0f;
}

PlasmaBullet::~PlasmaBullet()
{
    XTRACE();
}

void PlasmaBullet::init( ParticleInfo *p)
{
    XTRACE();
    float dx = HeroS::instance()->lastXPos - p->position.x;
    float dy = HeroS::instance()->lastYPos - p->position.y;

    float d = 1.0f/sqrt(dx*dx+dy*dy);
    dx *= d;
    dy *= d;

    p->velocity.x = dx * GAME_STEP_SCALE;
    p->velocity.y = dy * GAME_STEP_SCALE;
    p->velocity.z = 0 * GAME_STEP_SCALE;

    p->extra.x = 7.1f * GAME_STEP_SCALE;
    p->extra.y = 0.0f;

    p->damage = 40;
    p->tod = -1;

    p->radius = _bmHalfWidth*0.05f;

    GameState::enemyBulletCount++;

    //init previous values for interpolation
    updatePrevs(p);
}

bool PlasmaBullet::update( ParticleInfo *p)
{
//    XTRACE();
    if( (p->tod == 0) || (fabs( p->position.y) > 50.0)) 
    {
	//death or particle reaches bottom of screen, it dies
	GameState::enemyBulletCount--;
	return false;
    }

    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    p->extra.y += p->extra.x;

    return true;
}

void PlasmaBullet::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{ 
    p->tod = 0; 
    AudioS::instance()->playSample( "sounds/whoop.wav");
}

void PlasmaBullet::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

    glColor4f(1.0f,1.0f,1.0f, 0.6f);

    bindTexture();

    glRotatef( pi.extra.y, 0,0,1);
    _bitmaps->DrawC( _bulletIndex0, 0, 0, 0.12f, 0.12f);

    glRotatef( -pi.extra.y*2.3f, 0,0,1);
    _bitmaps->DrawC( _bulletIndex1, 0, 0, 0.12f, 0.12f);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

BallOfFire::BallOfFire( void):BitmapParticleType( "BallOfFire")
{
    XTRACE();
    string ballOfFire;
    ballOfFire = "BallOfFire";
    _ballOfFire = _bitmaps->getIndex( ballOfFire);

    _bmHalfWidth = _bitmaps->getWidth( _ballOfFire)/2.0f;
    _bmHalfHeight= _bitmaps->getHeight( _ballOfFire)/2.0f;
}

BallOfFire::~BallOfFire()
{
    XTRACE();
}

void BallOfFire::init( ParticleInfo *p)
{
    XTRACE();
    float dx = HeroS::instance()->lastXPos - p->position.x;
    float dy = HeroS::instance()->lastYPos - p->position.y;

    float d = 1.0f/sqrt(dx*dx+dy*dy);
    dx *= d;
    dy *= d;

    p->velocity.x = dx * GAME_STEP_SCALE;
    p->velocity.y = dy * GAME_STEP_SCALE;
    p->velocity.z = 0 * GAME_STEP_SCALE;

    if( dy < 0)
	p->extra.y = 180.0f + (asin( dx)*180.0f/(float)M_PI);
    else
	p->extra.y = -(asin( dx)*180.0f/(float)M_PI);

    p->damage = 40;
    p->tod = -1;

    p->radius = _bmHalfWidth*0.50f;

    GameState::enemyBulletCount++;

    //init previous values for interpolation
    updatePrevs(p);
}

bool BallOfFire::update( ParticleInfo *p)
{
//    XTRACE();
    if( (p->tod == 0) || (fabs( p->position.y) > 50.0)) 
    {
	//death or particle reaches bottom of screen, it dies
	GameState::enemyBulletCount--;
	return false;
    }

    static ParticleGroup *effects = 
	ParticleGroupManagerS::instance()->getParticleGroup( EFFECTS_GROUP2);
    int sparkType = Random::random()%3;
    switch( sparkType)
    {
	case 0:
	    effects->newParticle( 
		"FireSpark1", p->position.x, p->position.y, p->position.z);
	    break;
	case 1:
	    effects->newParticle( 
		"FireSpark2", p->position.x, p->position.y, p->position.z);
	    break;
	case 2:
	default:
	    effects->newParticle( 
		"FireSpark3", p->position.x, p->position.y, p->position.z);
	    break;
    }

    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    return true;
}

void BallOfFire::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{ 
    p->tod = 0; 
    AudioS::instance()->playSample( "sounds/whoop.wav");
}

void BallOfFire::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

    glColor4f(1.0f,1.0f,1.0f, 0.9f);

    bindTexture();

    glRotatef( pi.extra.y, 0,0,1);
    _bitmaps->DrawC( _ballOfFire, 0, 0, 0.12f, 0.12f);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

//------------------------------------------------------------------------------

StingerTrail::StingerTrail( void):
    ParticleType( "StingerTrail")
{
    XTRACE();
    _trail = ModelManagerS::instance()->getModel("models/P1");
}

StingerTrail::~StingerTrail()
{
    XTRACE();
}

void StingerTrail::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.x = (float)((Random::random()&0xf)-7) * 0.02f * GAME_STEP_SCALE;
    p->velocity.y = (float)((Random::random()&0xf)-7) * 0.02f * GAME_STEP_SCALE;
    p->velocity.z = (float)((Random::random()&0xf)-7) * 0.02f * GAME_STEP_SCALE;

    p->extra.x = 0.03f * GAME_STEP_SCALE;
    p->extra.y = 1.0f;
    p->extra.z = 0;

    p->tod = -1;

    //init previous values for interpolation
    updatePrevs(p);
}

bool StingerTrail::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->position.y > 50.0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->extra.y -= p->extra.x;
    if( p->extra.y <= 0) return false;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;

    p->extra.z += 7.5f*GAME_STEP_SCALE;

    return true;
}

void StingerTrail::draw( ParticleInfo *p)
{
    ParticleInfo pi;
    interpolate( p, pi);

//    XTRACE();
//	glColor4f(0.9*pi.extra.y,0.6*pi.extra.y,0.2*pi.extra.y, pi.extra.y);
//	glColor4f(0.9,0.6,0.2, pi.extra.y);
    glColor4f(0.3f,1.0f,0.2f, pi.extra.y);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);
    glRotatef(pi.extra.z, 1,1,0);

    _trail->draw();

    glPopMatrix();
}

//------------------------------------------------------------------------------

HeroStinger::HeroStinger( void):
    ParticleType( "HeroStinger")
{
    XTRACE();
    _stinger = ModelManagerS::instance()->getModel("models/HeroStinger");
}

HeroStinger::~HeroStinger()
{
    XTRACE();
}

void HeroStinger::init( ParticleInfo *p)
{
    XTRACE();
    p->velocity.x = 0 * GAME_STEP_SCALE;
    p->velocity.y = 4.5f * GAME_STEP_SCALE;
    p->velocity.z = 0 * GAME_STEP_SCALE;

    p->radius = getPseudoRadius( _stinger);

    p->damage = 100;
    p->tod = -1;

#ifdef TRAIL_3D
    p->extra.y = p->position.y;
#else
    p->extra.y = p->position.y + 3.0f;
#endif

    //init previous values for interpolation
    updatePrevs(p);
}

bool HeroStinger::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;
    if( p->position.y > 50.0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;

    return true;
}

void HeroStinger::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    //get particle group for special effects
    static ParticleGroup *effects = 
	ParticleGroupManagerS::instance()->getParticleGroup( EFFECTS_GROUP2);

//Note that we don't want the interpolated extra.y here!
//We want to start the trail at the current display position (pi)
#ifdef TRAIL_3D
    if( (pi.position.y - p->extra.y)*GameState::horsePower > (Random::random()&0xff))
    {

	effects->newParticle( 
	    "StingerTrail", pi.position.x, pi.position.y, pi.position.z);
	p->extra.y = pi.position.y;
    }
#else 
//	LOG_ERROR << p->extra.y << endl;
    if(  pi.position.y > p->extra.y)
    { 
	effects->newParticle( 
	    "SmokePuff", pi.position.x, pi.position.y, pi.position.z);
	p->extra.y = pi.position.y + 3.0f;
    }
#endif

    glColor4f(0.8f,0.8f,0.2f, 1.0f);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    _stinger->draw();

    glPopMatrix();
}

void HeroStinger::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{
//    XTRACE();
    p->tod = 0;
}

//------------------------------------------------------------------------------

StatusMessage::StatusMessage( void):
    ParticleType( "StatusMessage")
{
    XTRACE();
    _smallFont = FontManagerS::instance()->getFont( "bitmaps/arial-small");
    if( !_smallFont)
    {
	LOG_ERROR << "Unable to get font... (arial-small)" << endl;
    }
}

StatusMessage::~StatusMessage()
{
    XTRACE();
}

void StatusMessage::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.x = -1.0f* GAME_STEP_SCALE;

    p->extra.x = _smallFont->GetWidth( p->text.c_str(), 0.1f);
    p->position.x = 70.0f;

    LOG_INFO << "StatusMsg = [" << p->text << "] " /*<< p->position.y*/ << endl;

    p->tod = -1;

    //init previous values for interpolation
    updatePrevs(p);
}

bool StatusMessage::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

    p->position.x += p->velocity.x;

    if( p->position.x < -(70.0f+p->extra.x))
    {
	return false;
    }

    return true;
}

void StatusMessage::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    glColor4f( p->color.x, p->color.y, p->color.z, 0.8f);
    _smallFont->DrawString(
	    p->text.c_str(), 0 ,0 , p->extra.y, p->extra.z);

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------------

ExplosionPiece::ExplosionPiece( void):
    ParticleType( "ExplosionPiece")
{
    XTRACE();
    _cloud = ModelManagerS::instance()->getModel("models/E1");
}

ExplosionPiece::~ExplosionPiece()
{
    XTRACE();
}

void ExplosionPiece::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.x = (float)((Random::random()&0xff)-128)*0.002f* GAME_STEP_SCALE;
    p->velocity.y = (float)((Random::random()&0xff)-128)*0.002f* GAME_STEP_SCALE;
    p->velocity.z = (float)((Random::random()&0xff)-128)*0.002f* GAME_STEP_SCALE;

    p->extra.x = ((float)(Random::random()%90)-45.0f)*0.1f * GAME_STEP_SCALE;
    p->extra.y = 1.0f;
    p->tod = -1;

    p->extra.z = 0;

    //init previous values for interpolation
    updatePrevs(p);
}

bool ExplosionPiece::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

    p->extra.y -= 0.03f * GAME_STEP_SCALE;
    if( p->extra.y <= 0) return false;

    p->extra.z += p->extra.x;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;

    return true;
}

void ExplosionPiece::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glColor4f(1.0f,0.2f,0.1f, pi.extra.y);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);
    glRotatef(pi.extra.z, 1,1,0);

    _cloud->draw();

    glPopMatrix();
}

//------------------------------------------------------------------------------

EnergyBlob::EnergyBlob( void):
    ParticleType( "EnergyBlob")
{
    XTRACE();
    _blob = ModelManagerS::instance()->getModel("models/EnergyBlob");
}

EnergyBlob::~EnergyBlob()
{
    XTRACE();
}

void EnergyBlob::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.y = -1.30f * GAME_STEP_SCALE;
    p->extra.x = Random::rangef0_1()*60.0f - 30.0f;
    p->extra.y = (Random::rangef0_1()*30.0f + 20.0f)*0.1f * GAME_STEP_SCALE;

    p->tod = -1;
    p->damage = 0;

    p->radius = getPseudoRadius( _blob);

    //init previous values for interpolation
    updatePrevs(p);
}

bool EnergyBlob::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;
    if( fabs( p->position.y) > 50.0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.y += p->velocity.y;
    p->extra.x += p->extra.y;

    return true;
}

void EnergyBlob::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{
    if( GameState::horsePower > 90.0)
    {
	static ParticleGroup *effects = 
	    ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);

	ParticleInfo pi;
	pi.color.x = 0.5f;
	pi.color.y = 0.5f;
	pi.color.z = 1.0f;
	pi.position = p->position;
	pi.text = "Energy";
	effects->newParticle( "ScoreHighlight", pi);
    }

    AudioS::instance()->playSample( "sounds/voiceEnergy.wav");
    HeroS::instance()->addEnergy( 50);
    p->tod = 0;
}

void EnergyBlob::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);
    glRotatef( pi.extra.x, 1,0,0);
    _blob->draw();

    glPopMatrix();

}

//------------------------------------------------------------------------------

ShieldBoost::ShieldBoost( void):
    ParticleType( "ShieldBoost")
{
    XTRACE();
    _shield = ModelManagerS::instance()->getModel("models/ShieldBoost");
}

ShieldBoost::~ShieldBoost()
{
    XTRACE();
}

void ShieldBoost::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.y = -1.30f * GAME_STEP_SCALE;
    p->extra.x = Random::rangef0_1()*60.0f - 30.0f;
    p->extra.y = (Random::rangef0_1()*30.0f + 20.0f)*0.1f * GAME_STEP_SCALE;

    p->tod = -1;
    p->damage = 0;

    p->radius = getPseudoRadius( _shield);

    //init previous values for interpolation
    updatePrevs(p);
}

bool ShieldBoost::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;
    if( fabs( p->position.y) > 50.0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.y += p->velocity.y;
    p->extra.x += p->extra.y;

    return true;
}

void ShieldBoost::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{
    if( GameState::horsePower > 90.0)
    {
	static ParticleGroup *effects = 
	    ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);

	ParticleInfo pi;
	pi.color.x = 1.0f;
	pi.color.y = 1.0f;
	pi.color.z = 0.0f;
	pi.position = p->position;
	pi.text = "Shield";
	effects->newParticle( "ScoreHighlight", pi);
    }

    AudioS::instance()->playSample( "sounds/voiceShield.wav");
    HeroS::instance()->addShield( 50);
    p->tod = 0;
}

void ShieldBoost::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);
    glRotatef( pi.extra.x, 0,1,0);
    _shield->draw();

    glPopMatrix();

}

//------------------------------------------------------------------------------

ArmorPierce::ArmorPierce( void):
    ParticleType( "ArmorPierce")
{
    XTRACE();
    _armor = ModelManagerS::instance()->getModel("models/ArmorPierce");
}

ArmorPierce::~ArmorPierce()
{
    XTRACE();
}

void ArmorPierce::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.y = -1.30f * GAME_STEP_SCALE;
    p->extra.x = Random::rangef0_1()*60.0f - 30.0f;
    p->extra.y = (Random::rangef0_1()*30.0f + 20.0f)*0.1f * GAME_STEP_SCALE;

    p->tod = -1;
    p->damage = 0;

    p->radius = getPseudoRadius( _armor);

    //init previous values for interpolation
    updatePrevs(p);
}

bool ArmorPierce::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;
    if( fabs( p->position.y) > 50.0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.y += p->velocity.y;
    p->extra.x += p->extra.y;

    return true;
}

void ArmorPierce::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{
    if( GameState::horsePower > 90.0)
    {
	static ParticleGroup *effects = 
	    ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);

	ParticleInfo pi;
	pi.color.x = 0.0f;
	pi.color.y = 1.0f;
	pi.color.z = 1.0f;
	pi.position = p->position;
	pi.text = "Armor Pierce";
	effects->newParticle( "ScoreHighlight", pi);
    }

    //FIXME: for now just give some points...
    ScoreKeeperS::instance()->addToCurrentScore( 1000);

    AudioS::instance()->playSample( "sounds/prriarr.wav");
    HeroS::instance()->setArmorPierce( 2.0f);
    p->tod = 0;
}

void ArmorPierce::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);
    glRotatef( pi.extra.x, 1,1,0);
    _armor->draw();

    glPopMatrix();

}

//------------------------------------------------------------------------------

WeaponUpgrade::WeaponUpgrade( void):
    ParticleType( "WeaponUpgrade")
{
    XTRACE();
    _upgrade = ModelManagerS::instance()->getModel("models/WeaponUpgrade");
}

WeaponUpgrade::~WeaponUpgrade()
{
    XTRACE();
}

void WeaponUpgrade::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.y = -1.30f * GAME_STEP_SCALE;
    p->extra.x = Random::rangef0_1()*60.0f - 30.0f;
    p->extra.y = (Random::rangef0_1()*30.0f + 20.0f)*0.1f * GAME_STEP_SCALE;

    p->tod = -1;
    p->damage = 0;

    p->radius = getPseudoRadius( _upgrade);

    //init previous values for interpolation
    updatePrevs(p);
}

bool WeaponUpgrade::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;
    if( fabs( p->position.y) > 50.0) return false;

    //update previous values for interpolation
    updatePrevs(p);

    p->position.y += p->velocity.y;
    p->extra.x += p->extra.y;

    return true;
}

void WeaponUpgrade::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{
    if( GameState::horsePower > 90.0)
    {
	static ParticleGroup *effects = 
	    ParticleGroupManagerS::instance()->getParticleGroup(EFFECTS_GROUP2);

	ParticleInfo pi;
	pi.color.x = 1.0f;
	pi.color.y = 0.0f;
	pi.color.z = 0.0f;
	pi.position = p->position;
	pi.text = "WeaponUpgrade";
	effects->newParticle( "ScoreHighlight", pi);
    }

    //FIXME: for now just give some points...
    ScoreKeeperS::instance()->addToCurrentScore( 1000);

//    AudioS::instance()->playSample( "sounds/voiceUpgrade.wav");
//    HeroS::instance()->addShield( 50);
    p->tod = 0;
}

void WeaponUpgrade::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);
    glRotatef( pi.extra.x, 0,1,1);
    _upgrade->draw();

    glPopMatrix();

}

//------------------------------------------------------------------------------

Bonus1::Bonus1( string modelName, int value):
    ParticleType( modelName),
    _value(value)
{
    XTRACE();
    string fullModelName = "models/" + modelName;
    _bonus = ModelManagerS::instance()->getModel( fullModelName);
}

Bonus1::~Bonus1()
{
    XTRACE();
}

void Bonus1::init( ParticleInfo *p)
{
//    XTRACE();
    p->velocity.y = -1.30f * GAME_STEP_SCALE;
    p->extra.x = Random::rangef0_1()*60.0f - 30.0f;
    p->extra.y = (Random::rangef0_1()*30.0f + 20.0f)*0.1f * GAME_STEP_SCALE;

    p->tod = -1;
    p->damage = 0;

    p->radius = getPseudoRadius( _bonus);

    //init previous values for interpolation
    updatePrevs(p);
}

bool Bonus1::update( ParticleInfo *p)
{
//    XTRACE();
    if( p->tod == 0) return false;
    if( fabs( p->position.y) > 50.0) 
    {
	ScoreKeeperS::instance()->incGoodiesMissed();
	return false;
    }

    //update previous values for interpolation
    updatePrevs(p);

    p->position.y += p->velocity.y;
    p->extra.x += p->extra.y;

    return true;
}

void Bonus1::hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/) 
{
//    XTRACE();
    static ParticleGroup *effects = 
	ParticleGroupManagerS::instance()->getParticleGroup( EFFECTS_GROUP2);

    static ParticleGroup *bonus =
	ParticleGroupManagerS::instance()->getParticleGroup( BONUS_GROUP);

    p->tod = 0;

    int newValue = ScoreKeeperS::instance()->addToCurrentScore( _value);
    ScoreKeeperS::instance()->incGoodiesCaught();

    ParticleInfo pi;
    pi.position = p->position;
    char buf[10];
    sprintf( buf, "%d", newValue);
    pi.text = buf;

    if( newValue < 1000)
    {
	AudioS::instance()->playSample( "sounds/pop.wav");
	if( GameState::horsePower > 90.0)
	{
	    pi.color.x = 1.0f;
	    pi.color.y = 1.0f;
	    pi.color.z = 1.0f;

	    effects->newParticle( "ScoreHighlight", pi);
	}
    }
    else
    {
	AudioS::instance()->playSample( "sounds/bonus.wav");

	pi.color.x = 1.0f;
	pi.color.y = 0.2f;
	pi.color.z = 0.2f;

	effects->newParticle( "ScoreHighlight", pi);

	if( (Random::random() & 0xff) < 0x80)
	    bonus->newParticle(
		"ShieldBoost", p->position.x, 50.0f, p->position.z);
	else
	    bonus->newParticle(
		"EnergyBlob", p->position.x, 50.0f, p->position.z);
    }
//	LOG_INFO << "Bonus: " << newValue << endl;
}

void Bonus1::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);
    glRotatef( pi.extra.x, 0,1,1);
    _bonus->draw();

    glPopMatrix();
}

//------------------------------------------------------------------------------

ScoreHighlight::ScoreHighlight( void):
    ParticleType( "ScoreHighlight")
{
    _font = FontManagerS::instance()->getFont("bitmaps/arial-small");
    if( !_font)
    {
	LOG_ERROR << "Unable to get font... (arial-small)" << endl;
    }
}

ScoreHighlight::~ScoreHighlight()
{
}

void ScoreHighlight::init( ParticleInfo *p)
{
    p->velocity.x = (float)((Random::random()&0xff)-168)*0.003f* GAME_STEP_SCALE;
    p->velocity.y = (float)((Random::random()&0xff)-128)*0.002f* GAME_STEP_SCALE;
    p->velocity.z = (float)((Random::random()&0xff)-128)*0.002f* GAME_STEP_SCALE;

    p->extra.x = Random::rangef0_1()*40.0f-20.0f;
    p->extra.y = 0.05f;
    p->extra.z = 0.8f;

    p->tod = -1;

    //init previous values for interpolation
    updatePrevs(p);
}

bool ScoreHighlight::update( ParticleInfo *p)
{
//    XTRACE();
    //update previous values for interpolation
    updatePrevs(p);

    p->extra.z -= 0.02f * GAME_STEP_SCALE;
    //if alpha reaches 0, we can die
    if( p->extra.z < 0) return false;

    p->extra.x += 1.00f * GAME_STEP_SCALE;
    p->extra.y += 0.005f * GAME_STEP_SCALE;

    p->position.x += p->velocity.x;
    p->position.y += p->velocity.y;
    p->position.z += p->velocity.z;

    return true;
}

void ScoreHighlight::draw( ParticleInfo *p)
{
//    XTRACE();
    ParticleInfo pi;
    interpolate( p, pi);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glPushMatrix();

    glTranslatef( pi.position.x, pi.position.y, pi.position.z);

    //rotate towards the camera
    CameraS::instance()->billboard();

//    glRotatef( pi.extra.x, 0,0,1);

    glColor4f( p->color.x, p->color.y, p->color.z, pi.extra.z);
    _font->DrawString( p->text.c_str(), 0, 0, pi.extra.y, pi.extra.y);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------------
