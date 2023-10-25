#pragma once
// Description:
//   Structures and classes for LevelPacks.
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
#include <Point.hpp>
#include <BezierCurve.hpp>

struct LevelInfo {
    string name;    //Name for Level or LevelPack
    string author;  //Author of Level or LevelPack
    float version;

#if 0
    //extra data...
    int numTags;
    string *tagName; 
    Value  *tagValue;
#endif
};

class LPath {
    //bezier curve or some constant velocity curve...
public:
    LPath(BezierCurve<Point3D>* bc, bool linear = false) :
        _isLinear(linear),
        _bc(bc) {
        XTRACE();
        _init();
    }

    LPath(void) :
        _isLinear(false),
        _bc(0) {}

    void init(BezierCurve<Point3D>* bc, bool linear = false) {
        XTRACE();
        _bc = bc;
        _isLinear = linear;
        _init();
    }

    ~LPath() {
        XTRACE();
        delete _bc;
    }

    float length(void) { return _length; }

    void getPos(float t, Point3D& pos) {
        //assume t is [0,_length]
        _bc->GetPos(t, pos);
    }

    void getTangent(float t, Point3D& tangent) {
        //assume t is [0,_length]
        _bc->GetTangent(t, tangent);
    }

    void start(Point3D& pos) { pos = _start; }

    void startT(Point3D& tangent) { tangent = _startT; }

    void end(Point3D& pos) { pos = _end; }

    void endT(Point3D& tangent) { tangent = _endT; }

    void update(void) {
        //update internally cached values
        _init();
    }

    BezierCurve<Point3D>* getBezierCurve(void) { return _bc; }

private:
    bool _isLinear;
    float _length;

    void _init(void) {
        XTRACE();
        int num = _bc->GetNumSegments();
        if (num > 0) {
            _length = (float)(num);
            _start = _bc->GetControlPoint(0);
            _startT = _bc->GetControlPoint(1) - _start;
            _end = _bc->GetControlPoint(_bc->GetNumControlPoints() - 1);
            _endT = _bc->GetControlPoint(_bc->GetNumControlPoints() - 2) - _end;
        } else {
            _length = 0.0;
        }
    }

    Point3D _start;
    Point3D _startT;
    Point3D _end;
    Point3D _endT;

    BezierCurve<Point3D>* _bc;
};

//spawnTime after level start at spawnPoint
//follow entryPath then transition to home.
//From home via attackPath away from home
//and via retreatPath towards home.
//Idlepath when home.
struct LEnemy {
    float spawnTime;     //time at which to enter level
    Point3D spawnPoint;  //start position
    Point3D home;        //home position

    //the flight path this enemy is capable of
    int numAttackPaths;
    LPath** attack;

    int numRetreatPaths;
    LPath** retreat;

    //a single entry and idle path
    LPath* entry;
    LPath* idle;

    string modelName;  //the modelName for this enemy
};

struct LevelDescription {
    LevelInfo info;  //info for the level

    int numEnemies;
    LEnemy* enemy;
};

struct LevelPack {
    LevelInfo info;  //info for the levelpack

    int numLevels;
    LevelDescription* level;
};
