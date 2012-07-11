#pragma once
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

#include <Point.hpp>
#include <LevelPack.hpp>
#include <ParticleGroup.hpp>

class BaseEnemy: public ParticleType
{
public:
    BaseEnemy( LEnemy *le, std::string name);
    virtual ~BaseEnemy();

    virtual bool update( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/);

    void updateOffset( void);
    void sendHome( void);
    void getNextPath( void);

protected:
    enum MoveState
    {
	eIdle,
	eEntry,
	eAttack,
	eRetreat,
	eTransition,
	eLAST
    };
    //transforms y axis to at vector
    void alignWith( Point3D &at);

    LEnemy *_lEnemy;
    int _energy;
    float _levelStartTime;
    float _pathStartTime;
    bool _enableDraw;
    bool _extraUpdateAfterTransport;
    Point3D _dir;
    Point3D _prevDir;
    Point3D _pos;
    Point3D _offset;
    MoveState _moveState;
    MoveState _nextMoveState;
    bool _moveHome;
    LPath *_activePath;
    LPath *_nextPath;
    int _activePathIndex;

    LPath _transitionPath;
    BezierCurve<Point3D> &_transitionBC;

    static ParticleGroup *_enemyGroup;
};
