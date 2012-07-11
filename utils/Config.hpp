#pragma once
// Description:
//   Singleton to Keep track of configurable values. 
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
#include <list>
#include <string>
#include <functional>

#include <Singleton.hpp>
#include <hashMap.hpp>
#include <HashString.hpp>

const std::string CONFIGFILE = "ConfigFile";

class Value;
class ConfigHandler;

class Config
{
friend class Singleton<Config>;
public:
    struct ConfigItem
    {
	std::string key;
	std::string value;
    };

    //Persistent keyword/values start with a "+", transitory with a "-"
    void updateFromCommandLine( int argc, char *argv[]);

    void updateFromFile( void);
    void updateKeyword( const char *keyword, const char *value);
    void updateKeyword( const std::string keyword, const std::string value);
    void updateKeyword( const std::string keyword, Value *value);
    void remove( const std::string &keyword);

    //Transitory items are not saved. They override persistent values.
    void updateTransitoryKeyword( const char *keyword, const char *value);
    void updateTransitoryKeyword( const std::string keyword, const std::string value);
    void updateTransitoryKeyword( const std::string keyword, Value *value);
    void removeTrans( const std::string &keyword);

    //when reading/writing the config file, registerd ConfigHandlers
    //will be notified (see ConfigHandler)
    void registerConfigHandler( ConfigHandler *ch)
    {
        _configHandlerList.insert( _configHandlerList.begin(), ch);
    }

    bool getString( const std::string &keyword, std::string &value);
    bool getInteger( const std::string &keyword, int &value);
    bool getFloat( const std::string &keyword, float &value);
    bool getBoolean( const std::string &keyword, bool &value);

    void saveToFile( bool truncate=true);
    void dump( void);
    void getConfigItemList( std::list<ConfigItem> &ciList);

    const std::string &getConfigFileName( void);
    void setDefaultConfigFileName( char *defaultName)
    {
	_defaultConfigFileName = defaultName;
    }
    void setSubdirectory( const char *dirname)
    {
	_subdir = dirname;
    }
    std::string getConfigDirectory( void)
    {
    	return _configDirectory;
    }

private:
    ~Config();
    Config( void);
    Config( const Config&);
    Config &operator=(const Config&);

    void removeImpl( const std::string &keyword,
	    hash_map< std::string, Value*, hash<std::string>, std::equal_to<std::string> > &kvmap);

    bool getStringImpl( const std::string &keyword, std::string &value, 
	    hash_map< std::string, Value*, hash<std::string>, std::equal_to<std::string> > &kvmap);
    bool getIntegerImpl( const std::string &keyword, int &value, 
	    hash_map< std::string, Value*, hash<std::string>, std::equal_to<std::string> > &kvmap);
    bool getBooleanImpl( const std::string &keyword, bool &value,
	    hash_map< std::string, Value*, hash<std::string>, std::equal_to<std::string> > &kvmap);
    bool getFloatImpl( const std::string &keyword, float &value,
	    hash_map< std::string, Value*, hash<std::string>, std::equal_to<std::string> > &kvmap);

    std::string _defaultConfigFileName;
    std::string _subdir;
    std::string _configDirectory;
    std::list<ConfigHandler*> _configHandlerList;
    hash_map< std::string, Value*, hash<std::string>, std::equal_to<std::string> > _keyValueMap;
    hash_map< std::string, Value*, hash<std::string>, std::equal_to<std::string> > _keyValueMapTrans;
};

typedef Singleton<Config> ConfigS;
