// Description:
//   Keeps track of configurable values.
//
// Copyright (C) 2001-2019 Frank Becker
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

#ifndef _MSC_VER
#include <unistd.h>
#else
#include <direct.h>
#endif

#include "Trace.hpp"
#include "ResourceManager.hpp"
#include "Tokenizer.hpp"
#include "Value.hpp"
#include "GetDataPath.hpp"

#include <memory>

using namespace std;

const string CONFIG_DEFAULT_FILE = "config.txt";

Config::Config(void) :
    _defaultConfigFileName(CONFIG_DEFAULT_FILE),
    _subdir(""),
    _configDirectory("") {
    XTRACE();
}

Config::~Config() {
    XTRACE();
}

void Config::getConfigItemList(list<ConfigItem>& ciList) {
    Yaml::Node& config = _yaml[DEFAULT_SECTION];

    for (auto itS = config.Begin(); itS != config.End(); itS++) {
        ConfigItem item;
        item.key = (*itS).first;
        item.value = (*itS).second.As<string>();
        ciList.insert(ciList.begin(), item);
    }
}

const string& Config::getConfigFileName(void) {
    if (_configFileName != "") {
        return _configFileName;
    }

    getString(CONFIGFILE, _configFileName);

    if (_configFileName == "") {
        _configFileName = _defaultConfigFileName;
    }

    if (_subdir != "") {
        _configDirectory = _subdir + "/";
        _configFileName = _configDirectory + _configFileName;
    }

    return _configFileName;
}

void Config::updateFromCommandLine(int argc, char* argv[]) {
    XTRACE();
    for (int i = 1; i < argc; i++) {
        if ((argv[i][0] == '+') && ((i + 1) < argc)) {
            updateKeyword(&argv[i][1], argv[i + 1]);
            i++;
        }
        if ((argv[i][0] == '-') && ((i + 1) < argc)) {
            if ((argv[i + 1][0] != '-') && (argv[i + 1][0] != '+')) {
                updateTransitoryKeyword(&argv[i][1], argv[i + 1]);
                i++;
            } else {
                updateTransitoryKeyword(&argv[i][1], "true");
            }
        } else if (argv[i][0] == '-') {
            updateTransitoryKeyword(&argv[i][1], "true");
        }
    }
}

void Config::updateFromFile(void) {
    XTRACE();

    string configFile = getConfigFileName();
    LOG_INFO << "Updating Configuration from : " << configFile << endl;
    if (!ResourceManagerS::instance()->hasResource(configFile)) {
        LOG_WARNING << configFile << " not found! Trying default..." << endl;
        configFile = "system/config.txt";
        if (!ResourceManagerS::instance()->hasResource(configFile)) {
            LOG_ERROR << configFile << " not found!" << endl;
            return;
        }
    }
    std::unique_ptr<ziStream> infilePtr(ResourceManagerS::instance()->getInputStream(configFile));
    ziStream& infile = *infilePtr;
    Yaml::Parse(_yaml, infile);
}

void Config::updateTransitoryKeyword(const string& keyword, const string& value) {
    XTRACE();
    removeTrans(keyword);
    _yamlTrans[DEFAULT_SECTION][keyword] = value;
}

void Config::updateKeyword(const string& keyword, const string& value, const string& section) {
    XTRACE();
    remove(keyword);
    removeTrans(keyword);  //also remove trans setting if it exists
    _yaml[section][keyword] = value;
}

void Config::updateTransitoryKeyword(const string& keyword, Value* value) {
    XTRACE();
    removeTrans(keyword);
    _yamlTrans[DEFAULT_SECTION][keyword] = value->getString();
}

void Config::updateKeyword(const string& keyword, Value* value, const string& section) {
    XTRACE();
    remove(keyword);
    removeTrans(keyword);  //also remove trans setting if it exists
    _yaml[section][keyword] = value->getString();
}

void Config::remove(const string& keyword) {
    _yaml[DEFAULT_SECTION].Erase(keyword);
}

void Config::removeTrans(const string& keyword) {
    _yamlTrans[DEFAULT_SECTION].Erase(keyword);
}

void Config::saveToFile(void) {
    XTRACE();

    const string configFile = getConfigFileName();
    LOG_INFO << "Saving Configuration to : " << configFile << endl;

    zoStream outfile(configFile.c_str());

    outfile << "# This is a generated file. Edit carefully!" << endl;

    Yaml::Serialize(_yaml, outfile);
}

template <typename T>
bool Config::get(const string& keyword, T& value) {
    XTRACE();
    Yaml::Node& node = _yamlTrans[DEFAULT_SECTION][keyword];
    if (node.IsNone()) {
        node = _yaml[DEFAULT_SECTION][keyword];
    }
    if (node.IsNone()) {
        return false;
    }
    value = node.As<T>();
    return true;
}

bool Config::getString(const string& keyword, string& value) {
    XTRACE();
    return get<string>(keyword, value);
}

bool Config::getInteger(const string& keyword, int& value) {
    XTRACE();
    return get<int>(keyword, value);
}

bool Config::getBoolean(const string& keyword, bool& value) {
    XTRACE();
    return get<bool>(keyword, value);
}

bool Config::getFloat(const string& keyword, float& value) {
    XTRACE();
    return get<float>(keyword, value);
}

void Config::dump(void) {
    XTRACE();
    Yaml::Node& config = _yaml[DEFAULT_SECTION];

    if (config.IsNone()) {
        LOG_ERROR << "config file empty?\n";
        return;
    }

    for (auto itS = config.Begin(); itS != config.End(); itS++) {
        if ((*itS).second.IsNone()) {
            break;
        }
        LOG_INFO << (*itS).first << " = " << (*itS).second.As<string>() << endl;
    }
}

bool Config::getList(const std::string& section, std::vector<ConfigItem>& items) {
    Yaml::Node& sectionNode = _yaml[section];

    if (sectionNode.IsNone()) {
        LOG_ERROR << "section '" << section << "' not found\n";
        return false;
    }

    for (auto itS = sectionNode.Begin(); itS != sectionNode.End(); itS++) {
        if ((*itS).second.IsNone()) {
            break;
        }
        ConfigItem ci;
        ci.key = (*itS).first;
        ci.value = (*itS).second.As<string>();
        items.push_back(ci);
    }

    return true;
}