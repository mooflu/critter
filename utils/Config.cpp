// Description:
//   Keeps track of configurable values. 
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
#include "Config.hpp"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef VCPP
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#include "Trace.hpp"
#include "ResourceManager.hpp"
#include "Tokenizer.hpp"
#include "ConfigHandler.hpp"
#include "Value.hpp"
#include "GetDataPath.hpp"

using namespace std;

const string CONFIG_DEFAULT_FILE = "config.txt";

Config::Config( void):
    _defaultConfigFileName(CONFIG_DEFAULT_FILE),
    _subdir(""),
    _configDirectory("")
{
    XTRACE();
}

Config::~Config()
{
    XTRACE();

    hash_map<string, Value*, hash<string> >::const_iterator ci;
    for( ci=_keyValueMap.begin(); ci!=_keyValueMap.end(); ci++)
    {
        delete ci->second;
    }
    _keyValueMap.clear();

    for( ci=_keyValueMapTrans.begin(); ci!=_keyValueMapTrans.end(); ci++)
    {
        delete ci->second;
    }
    _keyValueMapTrans.clear();
}

void Config::getConfigItemList( list<ConfigItem> &ciList)
{
    hash_map<string, Value*, hash<string> >::const_iterator ci;
    for( ci=_keyValueMap.begin(); ci!=_keyValueMap.end(); ci++)
    {
	ConfigItem item;
	item.key = ci->first;
	item.value = ci->second->getString();
	ciList.insert( ciList.begin(), item);
    }
}

const string &Config::getConfigFileName( void)
{
    static string configFile;
    if( configFile != "" ) return configFile;

    getString( CONFIGFILE, configFile);

    struct stat statInfo;
    //does explicitly given config file exist?
    if( configFile == "")
    {
	if( (stat( _defaultConfigFileName.c_str(), &statInfo) != -1) )
	{
	    //local config.txt exists -> OK
	    return _defaultConfigFileName;
	}

	configFile = _defaultConfigFileName;

	if( _subdir != "")
	{
	    _configDirectory = _subdir + "/";
	    configFile = _configDirectory + configFile;
	}

        string prefPath = getPreferencesPath();
	if( prefPath.length())
	{
	    _configDirectory = prefPath + string("/") + _configDirectory;
	    configFile = prefPath + string("/") + configFile;
	}
    }
    else
    {
	if( (stat( configFile.c_str(), &statInfo) != -1) )
	{
	    //local config exists -> OK
	    return configFile;
	}

	if( _subdir != "")
	{
	    _configDirectory = _subdir + "/";
	    configFile = _configDirectory + configFile;
	}

        string prefPath = getPreferencesPath();
	if( prefPath.length())
	{
	    string homeConfig = prefPath + string("/") + configFile;
	    if( (stat( homeConfig.c_str(), &statInfo) != -1) )
	    {
		_configDirectory = prefPath + string("/") + _configDirectory;
		//home config exists -> OK
		configFile = homeConfig;
	    }
	}
    }

    return configFile;
}

void Config::updateFromCommandLine( int argc, char *argv[])
{
    XTRACE();
    for( int i=1; i<argc; i++)
    {
        if( (argv[i][0] == '+') && ((i+1)<argc))
        {
            updateKeyword( &argv[i][1], argv[i+1]);
	    i++;
        }
        if( (argv[i][0] == '-') && ((i+1)<argc))
        {
	    if( (argv[i+1][0] != '-') && (argv[i+1][0] != '+'))
	    {
		updateTransitoryKeyword( &argv[i][1], argv[i+1]);
		i++;
	    }
	    else
		updateTransitoryKeyword( &argv[i][1], "true");
        }
	else if( (argv[i][0] == '-'))
	{
            updateTransitoryKeyword( &argv[i][1], "true");
	}
    }
}

void Config::updateFromFile( void)
{
    XTRACE();

    const string configFile = getConfigFileName();
    LOG_DEBUG << "Updating Configuration from : " << configFile << endl;
#ifndef DEMO
    if( !ResourceManagerS::instance()->selectResource( configFile))
#endif
    {
	string systemConfigFile = "system/config.txt";
	if( !ResourceManagerS::instance()->selectResource( systemConfigFile))
	{
	    LOG_ERROR << "System Config file not found!" << endl;
	    return;
	}
        LOG_WARNING << "Config file [" << configFile 
	            << "] not found, using default." << endl;
    }
    ziStream &infile = ResourceManagerS::instance()->getInputStream();

    string line;
    
    while( !getline( infile, line).eof())
    {
//	LOG_INFO << "[" << line << "]" << endl; 

        //explicitly skip comments
        if( line[0] == '#') continue;

        list<ConfigHandler*>::iterator i;
        for( i=_configHandlerList.begin(); i!=_configHandlerList.end(); i++)
        {
            ConfigHandler *ch = *i;
            ch->handleLine( line);
        }

        Tokenizer  t( line, " \t\n\r=");
        string setKeyname = t.next();
        if( setKeyname == "set")
        {
	    string keyword = t.next();
	    string value = t.next();

//	    LOG_INFO << "Keyword [" << keyword << "] = [" 
//                     << value << "]" << endl;

	    updateKeyword( keyword, value);
        }
    }
}

void Config::updateTransitoryKeyword( const char *keyword, const char *value)
{
    XTRACE();
    if( keyword && value)
    {
        string tmpKeyword( keyword);
        string tmpValue( value);
	removeTrans( tmpKeyword);
	_keyValueMapTrans[ tmpKeyword] = new Value( tmpValue);
    }
}

void Config::updateKeyword( const char *keyword, const char *value)
{
    XTRACE();
    if( keyword && value)
    {
        string tmpKeyword( keyword);
        string tmpValue( value);
	remove( tmpKeyword);
	_keyValueMap[ tmpKeyword] = new Value( tmpValue);
    }
}

void Config::updateTransitoryKeyword( const string keyword, const string value)
{
    XTRACE();
    removeTrans( keyword);
    _keyValueMapTrans[ keyword] = new Value( value);
}

void Config::updateKeyword( const string keyword, const string value)
{
    XTRACE();
    remove( keyword);
    _keyValueMap[ keyword] = new Value( value);
}

void Config::updateTransitoryKeyword( const string keyword, Value *value)
{
    XTRACE();
    removeTrans( keyword);
    _keyValueMapTrans[ keyword] = value;
}

void Config::updateKeyword( const string keyword, Value *value)
{
    XTRACE();
    remove( keyword);
    _keyValueMap[ keyword] = value;
}

void Config::remove( const string &keyword)
{
    removeImpl(keyword, _keyValueMap);
}

void Config::removeTrans( const string &keyword)
{
    removeImpl(keyword, _keyValueMapTrans);
}

void Config::removeImpl( const string &keyword,
	hash_map< string, Value*, hash<string>, std::equal_to<string> > &kvmap)
{
    hash_map<string, Value*, hash<string> >::iterator ci;
    ci = kvmap.find( keyword);

    if( ci!=kvmap.end())
    {
	delete ci->second;
	kvmap.erase( ci);
    }
}

void Config::saveToFile( bool truncate)
{
    XTRACE();

    const string configFile = getConfigFileName();
    LOG_DEBUG << "Saving Configuration to : " << configFile << endl;

    string::size_type start = configFile.find_last_of( '/');
    if( start > 0)
    {
	struct stat statInfo;
	string path = configFile.substr( 0, start);
	if( (stat( path.c_str(), &statInfo) == -1) )
	{
#ifndef VCPP //TODO: May need to implement this in the future...
	    mkdir( path.c_str(), 0777);
#else
	    _mkdir( path.c_str());
#endif
	}
    }

    ios::openmode om = ios::out | ios::app;
    if( truncate)
    {
        om = ios::out | ios::trunc;
    }

    ofstream outfile( configFile.c_str(), om);

    outfile << "# This is a generated file. Edit carefully!" << endl;
    outfile << "# --- Variable section --- " << endl;
 
    hash_map<string, Value*, hash<string> >::const_iterator ci;
    for( ci=_keyValueMap.begin(); ci!=_keyValueMap.end(); ci++)
    {
        Value *val = ci->second;

        if( val)
	{
	    string v = val->getString();
            if( v.find_first_of(" \t") != string::npos)
	    {
		//value contains whitespace
		outfile << "set " << ci->first << " = \"" << v << "\"" << endl;
	    }
	    else
	    {
		outfile << "set " << ci->first << " = " << v << endl;
	    }
	}
        else
	{
	    LOG_WARNING << "No value for key [" << ci->first << "]" << endl;
	}

    }

    list<ConfigHandler*>::iterator i;
    for( i=_configHandlerList.begin(); i!=_configHandlerList.end(); i++)
    {
	ConfigHandler *ch = *i;
	ch->save( outfile);
    }
}

bool Config::getString( const string &keyword, string &value)
{
    XTRACE();
    if( getStringImpl(keyword,value,_keyValueMapTrans)) return true;
    return getStringImpl(keyword,value,_keyValueMap);
}

bool Config::getInteger( const string &keyword, int &value)
{
    XTRACE();
    if( getIntegerImpl(keyword,value,_keyValueMapTrans)) return true;
    return getIntegerImpl(keyword,value,_keyValueMap);
}

bool Config::getBoolean( const string &keyword, bool &value)
{
    XTRACE();
    if( getBooleanImpl(keyword,value,_keyValueMapTrans)) return true;
    return getBooleanImpl(keyword,value,_keyValueMap);
}

bool Config::getFloat( const string &keyword, float &value)
{
    XTRACE();
    if( getFloatImpl(keyword,value,_keyValueMapTrans)) return true;
    return getFloatImpl(keyword,value,_keyValueMap);
}

bool Config::getStringImpl( const string &keyword, string &value, 
	hash_map< string, Value*, hash<string>, std::equal_to<string> > &kvmap)
{
    hash_map<string, Value*, hash<string> >::const_iterator ci;
    ci = kvmap.find( keyword);

    if( ci!=kvmap.end())
    {
        value = ci->second->getString();
        return true;
    }

    return false; 
}

bool Config::getIntegerImpl( const string &keyword, int &value, 
	hash_map< string, Value*, hash<string>, std::equal_to<string> > &kvmap)
{
    hash_map<string, Value*, hash<string> >::const_iterator ci;
    ci = kvmap.find( keyword);

    if( ci!=kvmap.end())
    {
        value = ci->second->getInteger();
        return true;
    }

    return false; 
}

bool Config::getBooleanImpl( const string &keyword, bool &value,
	hash_map< string, Value*, hash<string>, std::equal_to<string> > &kvmap)
{
    hash_map<string, Value*, hash<string> >::const_iterator ci;
    ci = kvmap.find( keyword);

    if( ci!=kvmap.end())
    {
        value = ci->second->getBoolean();
        return true;
    }

    return false; 
}

bool Config::getFloatImpl( const string &keyword, float &value,
	hash_map< string, Value*, hash<string>, std::equal_to<string> > &kvmap)
{
    hash_map<string, Value*, hash<string> >::const_iterator ci;
    ci = kvmap.find( keyword);

    if( ci!=kvmap.end())
    {
        value = ci->second->getFloat();
        return true;
    }

    return false; 
}

void Config::dump( void)
{
    XTRACE();
    hash_map<string, Value*, hash<string> >::const_iterator ci;

    for( ci=_keyValueMap.begin(); ci!=_keyValueMap.end(); ci++)
    {
        Value *val = ci->second;
        if( val)
	    LOG_INFO << ci->first << " = " << val->dump() << endl;
        else
	    LOG_INFO << ci->first << endl;

    }
}
