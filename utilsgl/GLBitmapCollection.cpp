// Description:
//   A Bitmap Collection contains many bitmaps in a single PNG file. 
//   Bitmap names, offsets, sizes, etc. are kept in a seperate data file.
//   Includes GL drawing methods.
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
#include "GLBitmapCollection.hpp"

#include <SDL/SDL_image.h>
#include <zrwops.hpp>

#include "Trace.hpp"
#include "FindHash.hpp"
#include "ResourceManager.hpp"

using namespace std;

//Load bitmap and data file
bool GLBitmapCollection::Load( const char *bitmapFile, const char *dataFile)
{
    XTRACE();
    SDL_Surface *img;

    if( ResourceManagerS::instance()->selectResource( string(bitmapFile)+".png") ||
        ResourceManagerS::instance()->selectResource( string(bitmapFile)) )
    {
        ziStream &bminfile = ResourceManagerS::instance()->getInputStream();        
        
        SDL_RWops *src = RWops_from_ziStream( bminfile);
        img = IMG_LoadPNG_RW( src);
        if( !img)
        { 
            LOG_ERROR << "Failed to load PNG image: [" << bitmapFile << "]" << endl;
            return false;
        }
        SDL_RWclose( src);
    }
#ifdef IPHONE
    else if( ResourceManagerS::instance()->selectResource( string(bitmapFile)+".pvr"))
    {
        // pvr generated from png:
        // /Developer/Platforms/iPhoneOS.platform/usr/bin/texturetool -e PVRTC --bits-per-pixel-4 -p test.png -o a.pvr atlas.png
        int compressedSize = 
        ResourceManagerS::instance()->getResourceSize(string(bitmapFile)+".pvr");
        
        img = (SDL_Surface*)malloc(sizeof(SDL_Surface));
        
        img->w = 1024;
        img->h = 1024;
        img->flags = SDL_SWSURFACE;
        img->flags |= SDL_SRCALPHA;
        img->pitch = 0;
        img->compressedSize = compressedSize;
        img->pixels = (char*)malloc(compressedSize);
        
        int r = 
        ResourceManagerS::instance()->readResource( img->pixels, compressedSize); 
        if( r != compressedSize)
        {
            LOG_ERROR << "Failed to load PVR image: [" << bitmapFile << "]" << endl;
            free( img->pixels);
            free( img);
            return false;
        }
    }    
#endif
    else
    {
        LOG_WARNING << "No Bitmap file [" << bitmapFile << "] not found." << endl;
        return false;        
    }
    LOG_DEBUG << "Bitmap loaded." << endl;

    //assuming texture is square
    _textureSize = (float)img->w;

    if( !ResourceManagerS::instance()->selectResource( string(dataFile)))
    {
        LOG_WARNING << "Bitmap data file [" << dataFile 
                    << "] not found." << endl;
        SDL_FreeSurface( img);
        return false;
    }
    ziStream &datainfile = ResourceManagerS::instance()->getInputStream();

    LOG_DEBUG << "Reading: [" << dataFile << "]." << endl;
    _bitmapCount = 0;
    bool fileRead = false;
    while( !fileRead)
    {
        BitmapInfo tmpBInfo;
        string line;
        if( getline( datainfile, line).eof()) break;

        int n = sscanf( line.c_str(), "%d %d %d %d %d %d [%[\t-~]31s]\n",
            &tmpBInfo.xoff,
            &tmpBInfo.yoff,
            &tmpBInfo.xpos,
            &tmpBInfo.ypos,
	    &tmpBInfo.width,
	    &tmpBInfo.height,
	    tmpBInfo.name);
        if( tmpBInfo.name[ strlen(tmpBInfo.name)-1] == ']')
        {
            tmpBInfo.name[ strlen(tmpBInfo.name)-1] = '\0';
        }
	LOG_DEBUG << "Read: [" << tmpBInfo.name << "] " << n << endl;
        if( n == 6)
        {
            //assume missing element is a space
            strcpy( tmpBInfo.name, " ");
            n++;
        }
	if( n != 7)
        {
	    LOG_WARNING << "Data file [" << dataFile 
                        << "] has incorrect format." << endl;
	    break;
        }
	else
	{
	    _bitmapInfo[ _bitmapCount] = tmpBInfo;
            string tmpName = tmpBInfo.name;
	    LOG_DEBUG << "New bitmap: [" << tmpName << "] " << endl;
            _bitmapInfoMap[ tmpName] = &_bitmapInfo[ _bitmapCount];    
	    _bitmapCount++;
	}

        if( _bitmapCount > MAX_BITMAPS)
        {
	    LOG_ERROR << "Maximum number (" << MAX_BITMAPS 
                      << ") of bitmaps per collection exceeded." << endl;
	    fileRead = true;
        }
    } 

    _bitmapCollection = new GLTexture( GL_TEXTURE_2D, img, false);
    LOG_DEBUG << "Bitmap read OK." << endl;

    return true;
}

//Get index of bitmap with the given name
int GLBitmapCollection::getIndex( const string & name)
{
    XTRACE();
    for( unsigned int i=0; i<_bitmapCount; i++)
    {
        if( name == _bitmapInfo[ i].name) return (int)i;
    }

    return -1;
}

//Draw using bitmap index
void GLBitmapCollection::Draw( 
    const unsigned int index, 
    const float &x, 
    const float &y, 
    const float &scalex, 
    const float &scaley,
    const float &z)
{
    if( index >= _bitmapCount) return;

    _Draw( _bitmapInfo[ index], x, y, z, scalex, scaley);
}

//Draw using bitmap name
void GLBitmapCollection::Draw( 
    const string &name, 
    const float &x, 
    const float &y, 
    const float &scalex, 
    const float &scaley,
    const float &z)
{
//    cout << "Drawing bitmap " << name << endl;
    BitmapInfo *bmInfo = findHash<const string>( name, _bitmapInfoMap);

    if( !bmInfo)
    {   
        cerr << "Unable to fill GLBitmapCollection::Draw request for "
             << name << endl;
        return;
    }

    _Draw( *bmInfo, x, y, z, scalex, scaley);
}

void GLBitmapCollection::Draw( 
        unsigned int index, 
        const vec2f &ll, 
        const vec2f &ur)
{
    const BitmapInfo &bitmapInfo = _bitmapInfo[ index];
    
    float tx,ty, fxsize, fysize;
    
    fxsize = (float)bitmapInfo.width / _textureSize;
    fysize = (float)bitmapInfo.height / _textureSize;
    tx = (float)bitmapInfo.xpos / _textureSize;
    ty = (float)bitmapInfo.ypos / _textureSize;
    
    GLfloat squareVertices[] = {
        ll.x()       ,ll.y(),
        ur.x()       ,ll.y(),
        ur.x()       ,ur.y(),
        ll.x()       ,ur.y(),
    };
    
    GLfloat squareTexCoords[] = {
        tx       ,ty+fysize,
        tx+fxsize,ty+fysize,
        tx+fxsize,ty,
        tx       ,ty
    };
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);    
}


//Draw using bitmap info
void GLBitmapCollection::_Draw( 
    const BitmapInfo &bitmapInfo, 
    const float &x, 
    const float &y, 
    const float &z, 
    const float &scalex, 
    const float &scaley)
{
    XTRACE();

    float tx,ty, fxsize, fysize, ay;
    float dxsize, dysize;

    dxsize = (float)bitmapInfo.width*scalex;
    dysize = (float)bitmapInfo.height*scaley;
    ay = (float)(bitmapInfo.height-bitmapInfo.yoff)*scaley;
    fxsize = (float)bitmapInfo.width / _textureSize;
    fysize = (float)bitmapInfo.height / _textureSize;
    tx = (float)bitmapInfo.xpos / _textureSize;
    ty = (float)bitmapInfo.ypos / _textureSize;
    
    GLfloat squareVertices[] = {
        x       ,y+ay       ,z,
        x+dxsize,y+ay       ,z,
        x+dxsize,y+ay-dysize,z,
        x       ,y+ay-dysize,z
    };
    
    GLfloat squareTexCoords[] = {
        tx       ,ty,
        tx+fxsize,ty,
        tx+fxsize,ty+fysize,
        tx       ,ty+fysize
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 0, squareVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
        
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

//Draw centered using bitmap index
void GLBitmapCollection::DrawC( 
    const unsigned int index, 
    const float &x, 
    const float &y, 
    const float &scalex, 
    const float &scaley,
    const float &z)
{
    if( index >= _bitmapCount) return;

    _DrawC( _bitmapInfo[ index], x, y, z, scalex, scaley);
}

//Draw centered using bitmap name
void GLBitmapCollection::DrawC( 
    const string &name, 
    const float &x, 
    const float &y, 
    const float &scalex, 
    const float &scaley,
    const float &z)
{
//    cout << "Drawing bitmap " << name << endl;
    BitmapInfo *bmInfo = findHash<const string>( name, _bitmapInfoMap);

    if( !bmInfo)
    {   
        cerr << "Unable to fill GLBitmapCollection::Draw request for "
             << name << endl;
        return;
    }

    _DrawC( *bmInfo, x, y, z, scalex, scaley);
}

//Draw centered using bitmap info
void GLBitmapCollection::_DrawC( 
    const BitmapInfo &bitmapInfo, 
    const float &x, 
    const float &y, 
    const float &z, 
    const float &scalex, 
    const float &scaley)
{
    XTRACE();

    float tx,ty, fxsize, fysize;
    float dxsize, dysize;

    dxsize = (float)bitmapInfo.width*scalex*0.5f;
    dysize = (float)bitmapInfo.height*scaley*0.5f;
    fxsize = (float)bitmapInfo.width / _textureSize;
    fysize = (float)bitmapInfo.height / _textureSize;
    tx = (float)bitmapInfo.xpos / _textureSize;
    ty = (float)bitmapInfo.ypos / _textureSize;

    GLfloat squareVertices[] = {
        x-dxsize,y+dysize,z,
        x+dxsize,y+dysize,z,
        x+dxsize,y-dysize,z,
        x-dxsize,y-dysize,z
    };
    
    GLfloat squareTexCoords[] = {
        tx       ,ty,
        tx+fxsize,ty,
        tx+fxsize,ty+fysize,
        tx       ,ty+fysize
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, squareVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, squareTexCoords);
        
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
