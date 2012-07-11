#pragma once
// Description:
//   Starfield with nebulas.
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

#include <Singleton.hpp>
#include <GLBitmapCollection.hpp>

const int NUM_STARS = 300;
const int NUM_NEBULAS = 3;

class Starfield
{
friend class Singleton<Starfield>;
private:
    struct Nebula
    {
        float x;
        float y;
        float z;
        float d;

	float r;
	float g;
	float b;

        float sizeX;
        float sizeY;
        float rot;
        float max;
    };

    struct Star
    {
	float x;
	float y;
	float z;
	float d;

	float r;
	float g;
	float b;
    };  

public:
    void init( float z);
    void update( void);
    void draw( bool showStars, bool showNebulas);
    void bindTexture( void) { _nebula->bind();}

private:
    ~Starfield();
    Starfield( void);
    Starfield( const Starfield&);
    Starfield &operator=(const Starfield&);

    void pickSize( Nebula &n);
    void findMax( Nebula &n);
    void pickColor( Nebula &n);
    void updatePrevs( void);

    GLBitmapCollection *_nebula;
    int _nebulaIndex;
    float _nebulaHalfWidth;
    float _nebulaHalfHeight;

    Star _starInfo[ NUM_STARS];
    float _prevStarInfoY[ NUM_STARS];

    Nebula _nebulaInfo[ NUM_NEBULAS];
    float _prevNebulaInfoY[ NUM_NEBULAS];
};

typedef Singleton<Starfield> StarfieldS;
