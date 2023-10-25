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
#include <vector>

#include <Singleton.hpp>
#include <hashMap.hpp>
#include <HashString.hpp>

#include <Yaml.hpp>

const std::string CONFIGFILE = "ConfigFile";
const string DEFAULT_SECTION = "config";

class Value;
class ConfigHandler;

class Config {
    friend class Singleton<Config>;

public:
    struct ConfigItem {
        std::string key;
        std::string value;
    };

    //Persistent keyword/values start with a "+", transitory with a "-"
    void updateFromCommandLine(int argc, char* argv[]);

    void updateFromFile(void);
    void updateKeyword(const std::string& keyword, const std::string& value,
                       const std::string& section = DEFAULT_SECTION);
    void updateKeyword(const std::string& keyword, Value* value, const std::string& section = DEFAULT_SECTION);
    void remove(const std::string& keyword);

    //Transitory items are not saved. They override persistent values.
    void updateTransitoryKeyword(const std::string& keyword, const std::string& value);
    void updateTransitoryKeyword(const std::string& keyword, Value* value);
    void removeTrans(const std::string& keyword);

    bool getString(const std::string& keyword, std::string& value);
    bool getInteger(const std::string& keyword, int& value);
    bool getFloat(const std::string& keyword, float& value);
    bool getBoolean(const std::string& keyword, bool& value);

    bool getList(const std::string& section, std::vector<ConfigItem>& items);

    void saveToFile(void);
    void dump(void);
    void getConfigItemList(std::list<ConfigItem>& ciList);

    const std::string& getConfigFileName(void);

    void setDefaultConfigFileName(char* defaultName) { _defaultConfigFileName = defaultName; }

    void setSubdirectory(const std::string& dirname) { _subdir = dirname; }

    std::string getConfigDirectory(void) { return _configDirectory; }

private:
    ~Config();
    Config(void);
    Config(const Config&);
    Config& operator=(const Config&);

    template <typename T>
    bool get(const std::string& keyword, T& value);

    std::string _configFileName;

    std::string _defaultConfigFileName;
    std::string _subdir;
    std::string _configDirectory;

    Yaml::Node _yaml;
    Yaml::Node _yamlTrans;
};

typedef Singleton<Config> ConfigS;
