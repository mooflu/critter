// Description:
//   Online update check.
//
#ifdef USE_ONLINE_UPDATE
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
#include "OnlineUpdate.hpp"

#ifdef HAVE_CONFIG_H
#include <defines.h>
#endif

#include <Config.hpp>
#include <OSName.hpp>
#include <Constants.hpp>
#include <zStream.hpp>
#include <gettimeofday.hpp>

#include <curl/curl.h>
#include <tinyxml.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <sstream>

using namespace std;

int const UPDATE_INTERVAL = 60*60*24*7; //once per week

OnlineUpdate::OnlineUpdate( void):
  _thread(0),
  _status( eNotChecking),
  _isLatest(true)
{
}

OnlineUpdate::~OnlineUpdate()
{
    if( _thread)
    {
	LOG_INFO << "Waiting for online update thread to finish\n";
    	int status;
	SDL_WaitThread( _thread, &status);
	_thread = 0;
	LOG_INFO << "Online update thread done status = " << status << endl;
    }

    list<NewsItem*>::iterator i;
    for( i=_newsItemList.begin(); i!=_newsItemList.end(); i++)
    {
	delete (*i);
    }
    _newsItemList.clear();
}

void OnlineUpdate::init( const string &defaultUpdateURL)
{
    _updateURL = defaultUpdateURL;
    ConfigS::instance()->getString( "masterURL", _updateURL);
}

void OnlineUpdate::getUpdate( void)
{
    bool onlineCheck = false;
    ConfigS::instance()->getBoolean( "onlineCheck", onlineCheck);
    if( onlineCheck)
    {
	_text = "";
	_status = eDownloading;
#if 1
	_thread = SDL_CreateThread(OnlineUpdate::run, (void*)this);
	if ( _thread == NULL ) 
	{
	    _status = eFailure;
	    LOG_ERROR << "Unable to create thread.\n";
	}
#else
	OnlineUpdate::run( this);
#endif
    }
}

size_t OnlineUpdate::write(void *buffer, size_t size, size_t nmemb, void *data)
{
    char *tmpBuf = new char[size * nmemb +1];
    memcpy( tmpBuf, buffer, size * nmemb);
    tmpBuf[size * nmemb] = '\0';

    OnlineUpdate *onlineUpdate = (OnlineUpdate*)data;
    onlineUpdate->_text += tmpBuf;
//    LOG_INFO << tmpBuf << endl;

    delete [] tmpBuf;

    return size*nmemb;
}

void OnlineUpdate::loadUpdateCache(const string &updateCache)
{
    LOG_INFO << "Loading cached update\n";
    ifstream infile( updateCache.c_str(), ios::in | ios::binary);
    if( infile.good())
    {
	ziStream zin(infile);
	string line;
	while( !getline( zin, line).eof())
	{
	    _text += line;
	}

	if( process())
	    _status = eSuccess;
	else
	    _status = eFailure;
    }
}

int OnlineUpdate::run(void *data)
{
    OnlineUpdate *onlineUpdate = (OnlineUpdate*)data;
    string updateCache = ConfigS::instance()->getConfigDirectory() + "update.cache";

    struct stat statInfo;
    if( stat( updateCache.c_str(), &statInfo) != -1) 
    {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);

	int delta = tv.tv_sec - statInfo.st_mtime;
	if( delta < UPDATE_INTERVAL)
	{
	    onlineUpdate->loadUpdateCache( updateCache);
	    return 0;
	}
    }

    CURL *handle = curl_easy_init(); 
    if( handle)
    {
	stringstream useragent;
	useragent << PACKAGE << " " << VERSION << " " << OSNAME() << " (" << __DATE__ << " " << __TIME__ << ")" << ends;
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, OnlineUpdate::write);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(handle, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(handle, CURLOPT_USERAGENT, useragent.str().c_str());
	curl_easy_setopt(handle, CURLOPT_URL, onlineUpdate->_updateURL.c_str());
	//      curl_easy_setopt(handle, CURLOPT_REFERER, "None");

	LOG_INFO << "Checking for updates at " << onlineUpdate->_updateURL << endl;
	CURLcode success = curl_easy_perform(handle);
	if( success == CURLE_OK)
	{
	    //	    LOG_INFO << "Update text: [" << onlineUpdate->_text << "]\n";

	    if( onlineUpdate->process())
	    {
		ofstream outfile( updateCache.c_str(), ios::out | ios::binary);
		if( outfile.good())
		{
		    zoStream zout(outfile);
		    zout << onlineUpdate->_text;
		}
		onlineUpdate->_status = eSuccess;
	    }
	    else
		onlineUpdate->_status = eFailure;
	}
	else
	{
	    onlineUpdate->_status = eFailure;
	}

	curl_easy_cleanup(handle);
    }
    else
    {
	onlineUpdate->_status = eFailure;
    }

    if( onlineUpdate->_status != eSuccess)
    {
	LOG_WARNING << "Failed to get online update - trying cache.\n";
	onlineUpdate->loadUpdateCache( updateCache);
    }

    return 0;
}

int getVersionID( const string &version)
{
    int major = 0;
    int minor = 0;
    int patch = 0;
    sscanf( version.c_str(), "%d.%d.%d", &major, &minor, &patch);
    return major*10000 + minor*100 + patch;
}

bool OnlineUpdate::process( void)
{
    bool result = false;
    TiXmlDocument *doc = new TiXmlDocument();
    doc->Parse( _text.c_str());

    if( doc->Error())
    {
	LOG_ERROR << "Failed to parse xml content" << endl;
        LOG_ERROR << "--> XML: " << doc->ErrorDesc() << endl;
    }
    else
    {
	TiXmlNode *node;

	node = doc->FirstChild( "Program");
	while( node)
	{
	    TiXmlElement *element = node->ToElement();
	    string name = element->Attribute("Name");
	    if( name == GAMETITLE)
	    {
		LOG_INFO << "Found " << GAMETITLE << " update section\n";
		TiXmlNode *release = node->FirstChild( "Release");

		int thisVersionID = getVersionID(GAMEVERSION);
		int latestVersionID = 0;

		while( release)
		{
		    element = release->ToElement();
		    const char *version = element->Attribute("Version");
		    const char *date = element->Attribute("Date");
		    if( version && date)
		    {
			LOG_INFO << "Version " << version << " released on " << date << endl;

			int versionID = getVersionID(version);
			if( versionID > latestVersionID)
			{
			    latestVersionID = versionID;
			    _latestVersion = version;
			    _latestVersionDate = date;
			    TiXmlNode *body = release->FirstChild( "Body");
			    if( body)
			    {
				TiXmlText *leaf = body->FirstChild()->ToText();
				if( leaf)
				    _latestVersionText = leaf->Value();
			    }
			}

			if( thisVersionID == versionID)
			{
			    LOG_INFO << "Looking for config updates\n";

			    TiXmlNode *updateVarNode=release->FirstChild( "UpdateVar");
			    if( updateVarNode)
			    {
				TiXmlElement *updateVar=updateVarNode->ToElement();
				TiXmlAttribute *attribute = updateVar->FirstAttribute();
				while( attribute)
				{
				    string key = attribute->Name();
				    string value = attribute->Value();

				    LOG_INFO << "Update config " << key << "=" << value << endl;
				    ConfigS::instance()->updateKeyword( key, value);

				    attribute = attribute->Next();
				}
			    }
			}
		    }


		    release = release->NextSibling( "Release");
		}

		if( latestVersionID > thisVersionID)
		{
		    _isLatest = false;
		    LOG_INFO << "A newer version is available\n";
		    LOG_INFO << "Version " << _latestVersion << " released on " << _latestVersionDate << endl;
//		    LOG_INFO << "** " << _latestVersionText << endl;
		}
		break;
	    }
	    node = node->NextSibling( "Program");
	}

	node = doc->FirstChild( "NewsItem");
	while( node)
	{
	    TiXmlElement *element = node->ToElement();
	    const char *title = element->Attribute("Title");
	    const char *date = element->Attribute("Date");
	    if( title && date)
	    {
		TiXmlNode *body = element->FirstChild( "Body");
		if( body)
		{
		    TiXmlText *leaf = body->FirstChild()->ToText();
		    if( leaf)
		    {
			NewsItem *newsItem = new NewsItem;
			newsItem->title = title;
			newsItem->date = date;
			newsItem->text = leaf->Value();
			newsItem->r = 1.0;
			newsItem->g = 1.0;
			newsItem->b = 1.0;

			TiXmlElement *bodyElement = body->ToElement();
			TiXmlAttribute *attribute = bodyElement->FirstAttribute();
			while( attribute)
			{
			    string key = attribute->Name();
			    string value = attribute->Value();

			    if( key == "Color")
			    {
				sscanf( value.c_str(), "%f,%f,%f", 
					&newsItem->r, &newsItem->g, &newsItem->b);
			    }

			    attribute = attribute->Next();
			}

			LOG_INFO << "News: " << newsItem->title << " " << newsItem->date << endl;
			LOG_INFO << "  Text: " << newsItem->text << endl;

			_newsItemList.insert( _newsItemList.end(), newsItem);
		    }
		}
	    }
	    node = node->NextSibling( "NewsItem");
	}
	
	result = true;
    }

    delete doc;

    return result;
}
#endif
