#pragma once
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

#include <string>

#include <Point.hpp>
#include <GLBitmapFont.hpp>
#include <Model.hpp>
#include <ParticleType.hpp>
#include <BezierCurve.hpp>

class Particles
{
public:
    static void Initialize( void);
};

class BitmapParticleType: public ParticleType
{
public:
    BitmapParticleType( const std::string &name);
    virtual ~BitmapParticleType();

    virtual void init( ParticleInfo *p) = 0;
    virtual bool update( ParticleInfo *p) = 0;
    virtual void draw( ParticleInfo *p) = 0;

    static void bindTexture( void) { _bitmaps->bind();}

protected:
    static GLBitmapCollection *_bitmaps;
    void LoadBitmaps( void);
};

class SingleBitmapParticle: public BitmapParticleType
{
public:
    SingleBitmapParticle( const std::string &name, const char *bitmapName);

protected:
    float _bmHalfWidth;
    float _bmHalfHeight;
    float _radius;

    int _bmIndex;
};

class SmokePuff: public SingleBitmapParticle
{
public:
    SmokePuff( void);
    virtual ~SmokePuff();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

private:
};

class MiniSmoke: public SingleBitmapParticle
{
public:
    MiniSmoke( void);
    virtual ~MiniSmoke();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

private:
};

class Spark: public SingleBitmapParticle
{
public:
    Spark( void);
    virtual ~Spark();
    
    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    
private:
};

class FireSpark: public SingleBitmapParticle
{
public:
    FireSpark( const string &sp);
    virtual ~FireSpark();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

private:
};

class FlankBurst: public SingleBitmapParticle
{
public:
    FlankBurst( const string &sp);
    virtual ~FlankBurst();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

private:
    float _dir;
};

class BallOfPoison: public SingleBitmapParticle
{
public:
    BallOfPoison( void);
    virtual ~BallOfPoison();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

private:
};

class BallOfIce: public SingleBitmapParticle
{
public:
    BallOfIce( void);
    virtual ~BallOfIce();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

private:
};

class SwarmLeader: public SingleBitmapParticle
{
public:
    SwarmLeader( void);
    virtual ~SwarmLeader();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

private:
    BezierCurve<Point3D> _toHero;
};

class SwarmElement: public ParticleType
{
public:
    SwarmElement( void);
    virtual ~SwarmElement();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

private:
};

class Phaser: public SingleBitmapParticle
{
public:
    Phaser( void);
    virtual ~Phaser();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

private:
};

class PlasmaBullet: public BitmapParticleType
{
public:
    PlasmaBullet( void);
    virtual ~PlasmaBullet();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

private:
    int _bulletIndex0;
    int _bulletIndex1;

    float _bmHalfWidth;
    float _bmHalfHeight;
};

class BallOfFire: public BitmapParticleType
{
public:
    BallOfFire( void);
    virtual ~BallOfFire();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

private:
    int _ballOfFire;

    float _bmHalfWidth;
    float _bmHalfHeight;
};

class StingerTrail: public ParticleType
{
public:
    StingerTrail( void);
    virtual ~StingerTrail();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

protected:
    Model *_trail;
};

class HeroStinger: public ParticleType
{
public:
    HeroStinger( void);
    virtual ~HeroStinger();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);

protected:
    Model *_stinger;
};

class StatusMessage: public ParticleType
{
public:
    StatusMessage( void);
    virtual ~StatusMessage();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

protected:
    GLBitmapFont *_smallFont;
};  

class ExplosionPiece: public ParticleType
{
public:
    ExplosionPiece( void);
    virtual ~ExplosionPiece();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

protected:
    Model *_cloud;
};

class EnergyBlob: public ParticleType
{
public:
    EnergyBlob( void);
    virtual ~EnergyBlob();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);
    virtual void draw( ParticleInfo *p);

protected:
    Model *_blob;
};

class ShieldBoost: public ParticleType
{
public:
    ShieldBoost( void);
    virtual ~ShieldBoost();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);
    virtual void draw( ParticleInfo *p);

protected:
    Model *_shield;
};

class ArmorPierce: public ParticleType
{
public:
    ArmorPierce( void);
    virtual ~ArmorPierce();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);
    virtual void draw( ParticleInfo *p);

protected:
    Model *_armor;
};

class WeaponUpgrade: public ParticleType
{
public:
    WeaponUpgrade( void);
    virtual ~WeaponUpgrade();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);
    virtual void draw( ParticleInfo *p);

protected:
    Model *_upgrade;
};

class Bonus1: public ParticleType
{
public:
    Bonus1( string modelName, int value);
    virtual ~Bonus1();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int damage, int /*radIndex*/);
    virtual void draw( ParticleInfo *p);

protected:
    int _value;
    string _scoreName;
    Model *_bonus;
};

class ScoreHighlight: public ParticleType
{
public:
    ScoreHighlight( void);
    virtual ~ScoreHighlight();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);

protected:
    vec3 _color;
    std::string _value;
    GLBitmapFont *_font;
};
