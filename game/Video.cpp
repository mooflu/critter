// Description:
//   Video subsystem.
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
#include "Video.hpp"

#include <math.h>

#include <FPS.hpp>

#include <PNG.hpp>
#include <Trace.hpp>
#include <Random.hpp>
#include <Config.hpp>
#include <Value.hpp>
#include <GameState.hpp>
#include <Constants.hpp>

#include <Hero.hpp>
#include <ParticleGroupManager.hpp>
#include <Timer.hpp>
#include <Camera.hpp>
#include <Starfield.hpp>
#include <MenuManager.hpp>
#include <StageManager.hpp>
#include <FontManager.hpp>
#include <BitmapManager.hpp>
#include <ModelManager.hpp>
#include <ScoreKeeper.hpp>
#include <TextureManager.hpp>
#include <GLExtension.hpp>

#include <ResourceManager.hpp>
#include <zrwops.hpp>

#include "GLee.h"
#include "SDL/SDL.h"

#include <algorithm>
#include <string>
#include <locale>

using namespace std;

//Earlier versions of SDL don't have DISABLE defined
#ifndef SDL_DISABLE
#define SDL_DISABLE 0
#endif

Video::Video():
    _isFullscreen( false),
    _showStarfield( true),
    _showNebulas( true),
    _maxFPS(0),
    _fpsStepSize(0.0),
    _bpp( 0),
    _width( VIDEO_DEFAULT_WIDTH),
    _height( VIDEO_DEFAULT_HEIGHT),
    _boardVisible( true),
    _boardPosX( 0),
    _titleA( 0),
    _titleB( 0),
    _angle(0.0),
    _prevAngle(0.0)
{
    XTRACE();
}

Video::~Video()
{
    XTRACE();
    LOG_INFO << "Video shutdown..." << endl;

    BitmapManagerS::cleanup();
    FontManagerS::cleanup();
    TextureManagerS::cleanup();
    GLExtension::close();

    delete _titleA;
    delete _titleB;

    SkillS::cleanup();
    CameraS::cleanup();

#ifdef __APPLE__
    //Workaround for non-working minimize buttons on exit
    SDL_SetVideoMode( _width, _height, _bpp, 0);
#endif

    SDL_QuitSubSystem( SDL_INIT_VIDEO);
    SDL_Quit();
}

void Video::reset( void)
{
    ModelManagerS::instance()->reset();
}

void Video::reload( void)
{
    BitmapManagerS::instance()->reload();
    FontManagerS::instance()->reload();

    _titleA->reload();
    _titleB->reload();

    ModelManagerS::instance()->reload();
    MenuManagerS::instance()->reload();
}

bool Video::init( void)
{
    XTRACE();
    LOG_INFO << "Initializing Video..." << endl;

    if( SDL_InitSubSystem( SDL_INIT_VIDEO) < 0 )
    {
        LOG_ERROR << "Init Video: failed # " << SDL_GetError() << endl;
        return false;
    }
    LOG_INFO << "Video: OK" << endl;

    ConfigS::instance()->getInteger( "maxFPS", _maxFPS);
    if( _maxFPS)
    {
	LOG_INFO << "Video: Restricting FPS to " << _maxFPS << endl;
	_fpsStepSize = 1.0f/(float)_maxFPS;
    }

    ConfigS::instance()->getBoolean( "fullscreen", _isFullscreen);

    if( !setVideoMode())
    {
	SDL_QuitSubSystem( SDL_INIT_VIDEO);
	return false;
    }

    _smallFont = FontManagerS::instance()->getFont( "bitmaps/arial-small");
    if( !_smallFont)
    {
        LOG_ERROR << "Unable to get font... (arial-small)" << endl;
	SDL_QuitSubSystem( SDL_INIT_VIDEO);
        return false;
    }

    _scoreFont = FontManagerS::instance()->getFont( "bitmaps/vipnaUpper");
    if( !_scoreFont)
    {
        LOG_ERROR << "Unable to get font... (vipnaUpper)" << endl;
	SDL_QuitSubSystem( SDL_INIT_VIDEO);
        return false;
    }

    _gameOFont = FontManagerS::instance()->getFont( "bitmaps/gameover");
    if( !_gameOFont)
    {
        LOG_ERROR << "Unable to get font... (gameover)" << endl;
	SDL_QuitSubSystem( SDL_INIT_VIDEO);
        return false;
    }

    _board = BitmapManagerS::instance()->getBitmap( "bitmaps/board");
    if( !_board)
    {
	LOG_ERROR << "Unable to load CritterBoard" << endl;
	SDL_QuitSubSystem( SDL_INIT_VIDEO);
        return false;
    }
    _boardIndex = _board->getIndex( "CritterBoard");

    if( !ResourceManagerS::instance()->selectResource( 
	  string("bitmaps/titleA.png")))
    {
	LOG_WARNING << "titleA.png not found." << endl;
	return false;
    }
    ziStream &bminfile1 = ResourceManagerS::instance()->getInputStream();
    SDL_RWops *src = RWops_from_ziStream( bminfile1);
    SDL_Surface *img1 = IMG_LoadPNG_RW( src);
    SDL_RWclose( src);
    _titleA = new GLTexture( GL_TEXTURE_2D, img1, false);

    if( !ResourceManagerS::instance()->selectResource( 
	  string("bitmaps/titleB.png")))
    {
	LOG_WARNING << "titleB.png not found." << endl;
	return false;
    }
    ziStream &bminfile2 = ResourceManagerS::instance()->getInputStream();
    src = RWops_from_ziStream( bminfile2);
    SDL_Surface *img2 = IMG_LoadPNG_RW( src);
    SDL_RWclose( src);
    _titleB = new GLTexture( GL_TEXTURE_2D, img2, false);

    //hide&grab cursor and warp to centre
    SDL_ShowCursor( SDL_DISABLE);
    SDL_WM_GrabInput( SDL_GRAB_ON);
    SDL_WarpMouse( _width/2,  _height/2);

    SDL_Event event;
    while( SDL_PollEvent( &event))
    {
	//remove any queued up events due to warping, etc.
	;
    }

    LOG_INFO << "OpenGL info follows..." << endl;
    string vendor = (char*)glGetString( GL_VENDOR);
    LOG_INFO << "  Vendor  : " << vendor << endl;
    LOG_INFO << "  Renderer: " <<  glGetString( GL_RENDERER) << endl;
    LOG_INFO << "  Version : " << glGetString( GL_VERSION) << endl;

    glViewport(0,0, _width, _height);
/*
    GLfloat mat_ambient[]  = { 0.7, 0.7, 0.7, 0.0 };
    GLfloat mat_diffuse[]  = { 0.6, 0.6, 0.6, 0.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.0 };
*/

    GLfloat mat_ambient[]  = { 0.5f, 0.5f, 0.5f, 0.5f };
    GLfloat mat_diffuse[]  = { 0.4f, 0.4f, 0.4f, 0.5f };
    GLfloat mat_specular[] = { 0.6f, 0.6f, 0.6f, 0.5f };
    GLfloat mat_shininess[] = { 60.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    GLfloat mat_diffuse_b[]  = { 0.5f, 0.5f, 0.5f, 0.5f };
    GLfloat mat_ambient_b[]  = { 0.3f, 0.3f, 0.3f, 0.5f };
    GLfloat mat_specular_b[] = { 0.2f, 0.2f, 0.2f, 0.5f };
    GLfloat mat_shininess_b[] = { 10.0f };
    glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular_b);
    glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess_b);
    glMaterialfv(GL_BACK, GL_AMBIENT, mat_ambient_b);
    glMaterialfv(GL_BACK, GL_DIFFUSE, mat_diffuse_b);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    return true;
}

bool Video::setVideoMode( void)
{
    int videoFlags = SDL_OPENGL;

#if SDL_VERSION_ATLEAST(1, 2, 10)
    //Disable Vsync
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,0);
#endif

    ConfigS::instance()->getBoolean( "fullscreen", _isFullscreen);
    if( _isFullscreen)
    {
        LOG_INFO << "Fullscreen request." << endl;
        videoFlags |= SDL_FULLSCREEN;
    }

    if( !ConfigS::instance()->getInteger( "width", _width))
    {
	Value *w = new Value( _width);
	ConfigS::instance()->updateTransitoryKeyword( "width", w);
    }
    if( !ConfigS::instance()->getInteger( "height", _height))
    {
	Value *h = new Value( _height);
	ConfigS::instance()->updateTransitoryKeyword( "height", h);
    }

    if( SDL_SetVideoMode( _width, _height, _bpp, videoFlags ) == NULL )
    {
        LOG_ERROR << "Video Mode: failed #" << SDL_GetError() << endl;
        return false;
    }
    glViewport(0,0, _width, _height);

    //set title and icon name
    SDL_WM_SetCaption( "Critical Mass (aka Critter)", "Critical Mass" );

    SDL_Surface *surf = SDL_GetVideoSurface();

    LOG_INFO << "Video Mode: OK (" 
             << surf->w << "x" 
	     << surf->h << "x" 
	     << (int)surf->format->BitsPerPixel << ")" << endl;

    return true;
}

bool Video::updateSettings( void)
{
    bool fullscreen = _isFullscreen;
    ConfigS::instance()->getBoolean( "fullscreen", _isFullscreen);
    int width = 0;
    ConfigS::instance()->getInteger( "width", width);
    int height = 0;
    ConfigS::instance()->getInteger( "height", height);

    if( (fullscreen != _isFullscreen) || (width != _width) || (height != _height) )
    {
	reset();
	bool oldFullscreen = _isFullscreen;
	int oldWidth = _width;
	int oldHeight = _height;
	if( !setVideoMode())
	{
	    //new settings no good, try old
	    Value *fs = new Value( oldFullscreen);
	    ConfigS::instance()->updateKeyword( "fullscreen", fs);

	    Value *w = new Value( oldWidth);
	    ConfigS::instance()->updateKeyword( "width", w);

	    Value *h = new Value( oldHeight);
	    ConfigS::instance()->updateKeyword( "height", h);

	    if( !setVideoMode())
	    {
		//no luck, we are going down!
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		GameState::isAlive = false;
		return false;
	    }
	}
	reload();
    }

    ConfigS::instance()->getBoolean( "showStarfield", _showStarfield);
    ConfigS::instance()->getBoolean( "showNebulas", _showNebulas);

    return true;
}

void Video::updateLogic( void)
{
    _prevAngle = _angle;
    _angle += 5.0f;
    
    float boardWidth = (float)_board->getWidth( _boardIndex);    
    if( _boardVisible && (_boardPosX<0))
    {
        _boardPosX+=9;
        if( _boardPosX > 0) _boardPosX = 0;
    }
    
    if( !_boardVisible && (_boardPosX>-boardWidth))
    {
        _boardPosX-=9;
    }
}

bool Video::update( void)
{
    //    XTRACE();
    static float nextTime = Timer::getTime()+0.5f;
    float thisTime = Timer::getTime();
    if( thisTime > nextTime)
    {
	if( ! updateSettings()) return false;
	nextTime = thisTime+0.5f;
    }

    if( _maxFPS)
    {
	static float sTime = Timer::getTime();
	if( (thisTime-sTime) < _fpsStepSize)
	{
	    return true;
	}
	while(( thisTime-sTime) > _fpsStepSize) sTime+=_fpsStepSize;
    }

    FPS::Update();

    GLBitmapFont &smallFont = *_smallFont;
    GLBitmapFont &scoreFont = *_scoreFont;
    GLBitmapFont &gameOFont = *_gameOFont;

    glEnable( GL_DEPTH_TEST);
    glEnable( GL_LIGHTING);
    glEnable( GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    glEnable( GL_NORMALIZE);
//    glEnable( GL_RESCALE_NORMAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_COLOR_MATERIAL );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float fov = 53.13f;
    glFrustum( 
        (4.0/3.0)*(-2.0*tan(53.13 * M_PI / 360.0)),  //xmin
        (4.0/3.0)*( 2.0*tan(53.13 * M_PI / 360.0)),  //xmax
        -2.0*tan(53.13 * M_PI / 360.0),  //ymin
         2.0*tan(53.13 * M_PI / 360.0),  //ymax
         2.0,                            //znear
         1000.0);                        //zfar

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    CameraS::instance()->update();

    //place camera
    CameraS::instance()->place();

    GLfloat light_position[] = { 20.0, 0.0, -50.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    if( GameState::isDeveloper)
    {
	//highlight the playing field, useful when using mouselook
	glDisable( GL_DEPTH_TEST);
	glDisable( GL_LIGHTING);
	float y = tan(((float)M_PI*fov)/360.0f)*100;
	float x = y*4.0f/3.0f;
	glColor4f( 1.0f, 1.0f, 1.0f, 0.1f );
	glBegin(GL_QUADS);
	    glVertex3f( -x, y, -100);
	    glVertex3f(  x, y, -100);
	    glVertex3f(  x,-y, -100);
	    glVertex3f( -x,-y, -100);
	glEnd();
	glEnable( GL_LIGHTING);
	glEnable( GL_DEPTH_TEST);
    }

    StarfieldS::instance()->draw( _showStarfield, _showNebulas);

    if( GameState::context != Context::eMenu)
    {
	if( HeroS::instance()->alive())
	{
	    glPushMatrix();
	    HeroS::instance()->draw();
	    glPopMatrix();
	}

	ParticleGroupManagerS::instance()->draw();
    }

    //--- Ortho stuff from here on ---
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5,VIDEO_ORTHO_WIDTH+0.5,-0.5,VIDEO_ORTHO_HEIGHT+0.5, -1000.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor4f(1.0,1.0,1.0,1.0);
    bool showFPS = false;
    ConfigS::instance()->getBoolean( "showFPS", showFPS);
    if( showFPS)
    {
	smallFont.DrawString( FPS::GetFPSString(), 0, 0,  0.6f, 0.6f);
    }

    if( GameState::context != Context::eMenu)
    {
	if( !HeroS::instance()->alive())
	{
	    glColor4f(1.0f,1.0f,1.0f,0.8f);
	    gameOFont.DrawString( "GAME OVER" , 80, 320, 1.3f, 1.3f);

	    glColor4f(1.0f,0.0f,0.0f,0.8f);
	    smallFont.DrawString( "Too dangerous for ya? Try again!" , 
		215, 260, 1.0f, 1.0f);

            if( ScoreKeeperS::instance()->currentIsTopTen())
	    {
		if( !_textInput.isOn())
		{
		    _textInput.turnOn();
		}

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		smallFont.DrawString( "Top Ten Finish! RIP",215,200, 1.0f, 1.0f);

		string pname = "Enter Pilot name: ";
		pname += _textInput.getText()+"_";
		glColor4f(1.0f,0.852f,0.0f,1.0f);
		smallFont.DrawString( pname.c_str() , 215, 140, 1.0f, 1.0f);

		ScoreKeeperS::instance()->setName( _textInput.getText());
	    }
	    else
	    {
		string moremorequickquick = "Press right mouse button to restart";
		glColor4f(1.0f,0.852f,0.0f,1.0f);
		smallFont.DrawString( moremorequickquick.c_str() , 215, 140, 1.0f, 1.0f);
	    }

	    glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
    }

    char buff[128];
    if( GameState::isDeveloper)
    {
	static float nextShow = 0;
	static int aCount = 0;
	float thisTime = Timer::getTime();
	if( thisTime > nextShow)
	{
	    nextShow = thisTime + 0.5f;
	    aCount = ParticleGroupManagerS::instance()->getAliveCount();
	}
	sprintf( buff, "p=%d", aCount);
	smallFont.DrawString( buff, 0, 40, 1.0, 1.0);
    }

    if( GameState::context == Context::eMenu)
    {
	glEnable(GL_TEXTURE_2D);

	float z=-1.0;
	float dx= 1.0/512.0;

	_titleA->bind();
	glBegin(GL_QUADS);
	glTexCoord2f( dx     ,dx );   glVertex3f(350,740, z);
	glTexCoord2f( 1.0f-dx,dx );   glVertex3f(500,740, z);
	glTexCoord2f( 1.0f-dx,1-dx ); glVertex3f(500,560, z);
	glTexCoord2f( dx     ,1-dx ); glVertex3f(350,560, z);
	glEnd();

	_titleB->bind();
	glBegin(GL_QUADS);
	glTexCoord2f( dx     ,dx );   glVertex3f(500,740, z);
	glTexCoord2f( 1.0f-dx,dx );   glVertex3f(650,740, z);
	glTexCoord2f( 1.0f-dx,1-dx ); glVertex3f(650,560, z);
	glTexCoord2f( dx     ,1-dx ); glVertex3f(500,560, z);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	MenuManagerS::instance()->draw();

	glColor4f(1.0,1.0,1.0,0.5);
	string gVersion = "v"+GAMEVERSION;
	float width = smallFont.GetWidth( gVersion.c_str(), 0.6f);
	smallFont.DrawString( gVersion.c_str() , 995.0f-width, 5.0f, 0.6f, 0.4f);
    }
    else
    {
	float boardWidth = (float)_board->getWidth( _boardIndex);

        //draw board if at least partially visible
        if( _boardPosX > -boardWidth)
	{
	    float size = 0.48f;
	    float tdy = -36.0f;
	    float ty = (float)VIDEO_ORTHO_HEIGHT-56.0f;
	    float tx = 28.0f+_boardPosX;

	    glEnable(GL_TEXTURE_2D);
	    _board->bind();
	    glColor4f(1.0,1.0,1.0,0.5);
	    _board->Draw(_boardIndex,_boardPosX,VIDEO_ORTHO_HEIGHT-256,1.0,1.0);
	    glDisable(GL_TEXTURE_2D);

	    sprintf( buff, "%d", ScoreKeeperS::instance()->getCurrentScore());
	    scoreFont.DrawString( buff, tx, ty, size, size);
	    ty+=tdy;

	    sprintf( buff, "%d", ScoreKeeperS::instance()->getHighScore());
	    scoreFont.DrawString( buff, tx, ty, size, size);
	    ty+=tdy;

	    float he = HeroS::instance()->getEnergy();
	    if( he<0.0) he=0.0;
	    glColor4f( 1.0f, 0.1f, 0.1f, 0.5f );
	    glBegin(GL_QUADS);
		glVertex3f( tx        , ty+2, -1);
		glVertex3f( tx+he*.97f, ty+2, -1);
		glVertex3f( tx+he*.97f, ty+20, -1);
		glVertex3f( tx        , ty+20, -1);
	    glEnd();
	    glColor4f(1.0,1.0,1.0,0.5);

	    sprintf( buff, "%d", (int)he);
	    scoreFont.DrawString( buff, tx, ty, size, size);
	    ty+=tdy;

	    float se = HeroS::instance()->getShieldEnergy();
	    glColor4f( 1.0f, 0.8f, 0.0f, 0.5f );
	    glBegin(GL_QUADS);
		glVertex3f( tx        , ty+2, -1);
		glVertex3f( tx+se*.97f, ty+2, -1);
		glVertex3f( tx+se*.97f, ty+20, -1);
		glVertex3f( tx        , ty+20, -1);
	    glEnd();
	    glColor4f(1.0,1.0,1.0,0.5);

	    sprintf( buff, "%d", (int)se);
	    scoreFont.DrawString( buff, tx, ty, size, size);
	    ty+=tdy;

	    float we = HeroS::instance()->getWeaponEnergy()/(HeroS::instance()->getMaxWeaponEnergy()/100.0);
	    glColor4f( 0.2f, 1.0f, 0.2f, 0.5f );
	    glBegin(GL_QUADS);
		glVertex3f( tx        , ty+2, -1);
		glVertex3f( tx+we*.97f, ty+2, -1);
		glVertex3f( tx+we*.97f, ty+20, -1);
		glVertex3f( tx        , ty+20, -1);
	    glEnd();
	    glColor4f(1.0,1.0,1.0,0.5);
	    
	    sprintf( buff, "%d", (int)HeroS::instance()->getWeaponEnergy());
	    scoreFont.DrawString( buff, tx, ty, size, size);
	    ty+=tdy;

            SkillS::instance();
            string theSkill = Skill::getString(GameState::skill);
	    scoreFont.DrawString( theSkill.c_str(), tx, ty, size, size);

	    glEnable( GL_LIGHTING);
	    glEnable( GL_DEPTH_TEST);

	    GLfloat light_position2[] = { 820.0, 620.0, 500.0, 0.0 };
	    glLightfv(GL_LIGHT0, GL_POSITION, light_position2);
	    glDisable(GL_TEXTURE_2D);

	    float iAngle = _prevAngle+(_angle-_prevAngle)*GameState::frameFraction;

	    glPushMatrix();
	    glTranslatef(154.0+_boardPosX, 675.0, -1.0);
	    glRotatef(iAngle, 0.0, 1.0, 0.0);
	    HeroS::instance()->drawWeapon(0);
	    glPopMatrix();

	    glPushMatrix();
	    glTranslatef(154.0+_boardPosX, 621.0, -1.0);
	    glRotatef(iAngle, 0.0, 1.0, 0.0);
	    HeroS::instance()->drawWeapon(1);
	    glPopMatrix();

	    glPushMatrix();
	    glTranslatef(154.0+_boardPosX, 567.0, -1.0);
	    glRotatef(iAngle, 0.0, 1.0, 0.0);
	    HeroS::instance()->drawWeapon(2);
	    glPopMatrix();
	}
    }

    SDL_GL_SwapBuffers( );

    return true;
}

void Video::takeSnapshot( void)
{
    static int count = 0;

    int width = VideoS::instance()->getWidth();
    int height = VideoS::instance()->getHeight();
    char filename[128];
    sprintf( filename, "snap%02d.png", count++);
    SDL_Surface *img;

    img = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height, 24,
	    0xFF000000, 0x00FF0000, 0x0000FF00,0);

    glReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);

    LOG_INFO << "Writing snapshot: " << filename << endl;
    if( !PNG::Snapshot( img, filename))
    {
	LOG_ERROR << "Failed to save snapshot." << endl;
    }

    SDL_FreeSurface( img);
}
