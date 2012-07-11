// Description:
//   XML helper to help load xml docs.
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
#include "XMLHelper.hpp"

#include "Trace.hpp"
#include "ResourceManager.hpp"

using namespace std;

TiXmlDocument *XMLHelper::load( const string &filename)
{
    XTRACE();

    int rSize = ResourceManagerS::instance()->getResourceSize( filename);
    if( ! ResourceManagerS::instance()->selectResource( filename))
    {
	LOG_ERROR << "Unable to open: [" << filename << "]" << endl;
	return 0;
    }
    ziStream &infile = ResourceManagerS::instance()->getInputStream();

    char* buf = new char[rSize+1];
    infile.read( buf, rSize);
    streamsize c = infile.gcount();

//    LOG_INFO << "LevePack : " << filename 
//             << " (" << c << "/" << rSize << ")" << endl;
//    LOG_INFO << buf << endl;
    buf[ c] = '\0';

    TiXmlDocument *doc = new TiXmlDocument();
    doc->Parse( buf );
    delete [] buf;
//    doc->Print(stdout);

    if( doc->Error())
    {
	LOG_ERROR << "Failed to parse xml file: [" << filename << "]" << endl;
	LOG_ERROR << "--> XML: " << doc->ErrorDesc() << endl;
	delete doc;
	return 0;
    }

    return doc;
}
