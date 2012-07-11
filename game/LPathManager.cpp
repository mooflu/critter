// Description:
//   Path manager.
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
#include <LPathManager.hpp>

LPathManager::LPathManager( void)
{
    XTRACE();
}

LPathManager::~LPathManager()
{
    XTRACE();
}

LPath *LPathManager::load( const string &pathName)
{
    XTRACE();
    LPath *lp = 0;
    BezierCurve<Point3D> *bc = new BezierCurve<Point3D>(64);

    string fullPathName = pathName+".path";

    if( bc->Load( fullPathName.c_str()))
    {
	//move start of curve to origin
	Point3D start = bc->GetControlPoint(0) * -1;
	bc->MoveCurve( start);
	bc->ScaleCurve( 0.15f);
/*
	LOG_INFO << "Moving Curve by [" 
		 << start.x << "," 
		 << start.y << "," 
		 << start.z << "]" << endl; 
*/

	lp = new LPath( bc);
	LOG_INFO << "  Path [" << pathName << "]" << endl; 
    }
    else
    {
	LOG_ERROR << "Unable to load: " << pathName << endl;
	delete bc;
    }

    return lp;
}
