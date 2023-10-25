#pragma once
// Description:
//   Simple pak file type resouce manager.
//
// Copyright (C) 2011 Frank Becker
//
#include <list>
#include <string>

#include "Singleton.hpp"
#include "zStream.hpp"

class ResourceManager {
    friend class Singleton<ResourceManager>;

public:
    bool addResourceBundle(const std::string& bundle, const std::string& mountPoint);
    void setWriteDirectory(const std::string& writeDir);

    bool hasResource(const std::string& name);
    int getResourceSize(const std::string& name);
    ziStream* getInputStream(const std::string& name);

    void getFiles(const std::string& dirName, std::list<std::string>& results);

    void dump(void);

private:
    ResourceManager(const ResourceManager&);
    ResourceManager& operator=(const ResourceManager&);

    ResourceManager(void);
    ~ResourceManager();
};

typedef Singleton<ResourceManager> ResourceManagerS;
