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
#include "Starfield.hpp"

#include <math.h>
#include <string>

#include <Trace.hpp>
#include <GameState.hpp>
#include <Random.hpp>
#include <Constants.hpp>
#include <BitmapManager.hpp>

using namespace std;

Starfield::Starfield( void)
{
    XTRACE();

    _nebula = BitmapManagerS::instance()->getBitmap( "bitmaps/nebula");
    if( !_nebula)
    {
	LOG_ERROR << "Unable to load nebula" << endl;
    }
    _nebulaIndex = _nebula->getIndex( "Nebula");
    _nebulaHalfWidth = (float)(_nebula->getWidth( _nebulaIndex))/2.0f;
    _nebulaHalfHeight= (float)(_nebula->getHeight( _nebulaIndex))/2.0f;
}

Starfield::~Starfield()
{
    XTRACE();
}

void Starfield::init( float z)
{
    XTRACE();
    for( int i=0; i<NUM_STARS; i++)
    {
	_starInfo[i].x = Random::rangef0_1()*200.0f - 100.0f;
	_starInfo[i].y = Random::rangef0_1()*200.0f - 100.0f;
	_starInfo[i].z = z;
	_starInfo[i].d = Random::rangef0_1()*1.0f+0.25f;
    
	_starInfo[i].r = Random::rangef0_1()*0.5;
	_starInfo[i].g = Random::rangef0_1()*0.5;
	_starInfo[i].b = Random::rangef0_1()*0.5;
    }   
    for( int i=0; i<NUM_NEBULAS; i++)
    {
	pickSize( _nebulaInfo[i]);
	_nebulaInfo[i].rot = Random::rangef0_1() * 360.0f;
	findMax( _nebulaInfo[i]);

	_nebulaInfo[i].x   =(Random::rangef0_1() - 0.5f)*200.0f;
	_nebulaInfo[i].y   =(Random::rangef0_1() - 0.5f)*200.0f;
	_nebulaInfo[i].d   = Random::rangef0_1() * 1.50f+0.25f;

	_nebulaInfo[i].z = z + i;
	pickColor( _nebulaInfo[i]);
    }

    updatePrevs();
}

void Starfield::update( void)
{
//    XTRACE();
    updatePrevs();

    for( int i=0; i<NUM_STARS; i++)
    {   
	_starInfo[i].y -= _starInfo[i].d;
	if( _starInfo[i].y < -100.0)
	{
	    _starInfo[i].y = 100.0;
	    _prevStarInfoY[ i] = _starInfo[ i].y;
	}
    }
    for( int i=0; i<NUM_NEBULAS; i++)
    {   
	_nebulaInfo[i].y -= _nebulaInfo[i].d;
	if( _nebulaInfo[i].y < (-50.0-_nebulaInfo[i].max))
	{
	    pickSize( _nebulaInfo[i]);
	    _nebulaInfo[i].rot = Random::rangef0_1() * 360.0f;
	    findMax( _nebulaInfo[i]);

	    _nebulaInfo[i].x   =(Random::rangef0_1() - 0.5f)*200.0f;
	    _nebulaInfo[i].y   = 50.0f+_nebulaInfo[i].max;
	    _nebulaInfo[i].d   = Random::rangef0_1() * 1.50f+0.25f;

	    _prevNebulaInfoY[ i] = _nebulaInfo[ i].y;

	    pickColor( _nebulaInfo[i]);
	}
    }
}

void Starfield::updatePrevs( void)
{
//    XTRACE();
    for( int i=0; i<NUM_STARS; i++)
    {
	_prevStarInfoY[ i] = _starInfo[ i].y;
    }   
    for( int i=0; i<NUM_NEBULAS; i++)
    {
	_prevNebulaInfoY[ i] = _nebulaInfo[ i].y;
    }
}

void Starfield::draw( bool showStars, bool showNebulas)
{
//    XTRACE();
//	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    if( showStars)
    {
        GLfloat starVertices[NUM_STARS*3];
        GLfloat starColors[NUM_STARS*4];

	for( int i=0; i<NUM_STARS; i++)
	{
	    float vy = _prevStarInfoY[i] + 
                (_starInfo[i].y - _prevStarInfoY[i])* GameState::frameFractionOther;
            
            starVertices[i*3+0] = _starInfo[i].x; 
            starVertices[i*3+1] = vy; 
            starVertices[i*3+2] = _starInfo[i].z; 
            
            starColors[i*4+0] = _starInfo[i].r;
            starColors[i*4+1] = _starInfo[i].g;
            starColors[i*4+2] = _starInfo[i].b;
            starColors[i*4+3] = 0.7;
	}
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, starVertices);
        glColorPointer(4, GL_FLOAT, 0, starColors);
        
        glDrawArrays(GL_POINTS, 0, NUM_STARS);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    if( showNebulas)
    {
	glEnable(GL_TEXTURE_2D);
	bindTexture();
	for( int i=0; i<NUM_NEBULAS; i++)
	{
	    glPushMatrix();

	    float vy = _prevNebulaInfoY[i] +
		(_nebulaInfo[i].y - _prevNebulaInfoY[i]) * GameState::frameFractionOther;

	    glColor4f( _nebulaInfo[i].r, _nebulaInfo[i].g, _nebulaInfo[i].b, 0.3f);
	    glTranslatef( _nebulaInfo[i].x, vy, _nebulaInfo[i].z);
	    glRotatef( _nebulaInfo[i].rot, 0,0,1);

	    _nebula->DrawC( 
		_nebulaIndex, 0, 0, _nebulaInfo[i].sizeX, _nebulaInfo[i].sizeY);

	    glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
    }

    glEnable(GL_LIGHTING);
//	glEnable(GL_DEPTH_TEST);
}

void Starfield::pickSize( Nebula &n)
{
    n.sizeX = Random::rangef0_1()*1.2f + 0.4f;
    n.sizeY = Random::rangef0_1()*1.2f + 0.4f;
}

void Starfield::findMax( Nebula &n)
{
    float sina = _nebulaHalfWidth *n.sizeX*sin( n.rot * ((float)M_PI/180.0f));
    float cosa = _nebulaHalfHeight*n.sizeY*cos( n.rot * ((float)M_PI/180.0f));
    float t1 = fabs(-sina + cosa);
    float t2 = fabs(+sina + cosa);
    n.max = max(t1,t2);
}

void Starfield::pickColor( Nebula &n)
{
    //only use 2 color components
    float r1 = Random::rangef0_1()+0.1f;
    float r2 = Random::rangef0_1()+0.1f;

    float d = 1.8f/(r1+r2);
    r1 = r1*d;
    r2 = r2*d;
    
    int r = Random::random() % 3;
    switch( r)
    {
	case 0:
	    n.r = r1;
	    n.g = r2;
	    n.b = 0.0;
	    break;
	case 1:
	    n.r = 0.0;
	    n.g = r1;
	    n.b = r2;
	    break;
	default:
	case 2:
	    n.r = r1;
	    n.g = 0.0;
	    n.b = r2;
	    break;
    }
}
