// Description:
//   Simple pak file type resouce manager.
//
// Copyright (C) 2011 Frank Becker
//
#include "ResourceManager.hpp"
#include "GetDataPath.hpp"
#include "Trace.hpp"

#include <physfs.h>

#ifndef _MSC_VER
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

static void output_archivers(void) {
    const PHYSFS_ArchiveInfo** rc = PHYSFS_supportedArchiveTypes();
    const PHYSFS_ArchiveInfo** i;

    printf("Supported archive types:\n");
    if (*rc == NULL) {
        printf(" * Apparently, NONE!\n");
    } else {
        for (i = rc; *i != NULL; i++) {
            printf(" * %s: %s\n    Written by %s.\n    %s\n", (*i)->extension, (*i)->description, (*i)->author,
                   (*i)->url);
            printf("    %s symbolic links.\n", (*i)->supportsSymlinks ? "Supports" : "Does not support");
        } /* for */
    }     /* else */

    printf("\n");
} /* output_archivers */

ResourceManager::ResourceManager(void) {
    const char* argv0 = 0;
#if defined(EMSCRIPTEN)
    argv0 = "/dummy";
#endif
    if (PHYSFS_init(argv0) == 0) {
        LOG_ERROR << "Failed in initialize PHYSFS\n";
    } else {
        output_archivers();
    }
}

ResourceManager::~ResourceManager() {
    dump();
    PHYSFS_deinit();
}

void ResourceManager::dump(void) {
    // LOG_INFO << "PhysFS last error:" << PHYSFS_getLastErrorCode() << "\n";
    LOG_INFO << "Base Dir:" << PHYSFS_getBaseDir() << "\n";
    LOG_INFO << "Write Dir:" << PHYSFS_getWriteDir() << "\n";
    LOG_INFO << "Search Paths:\n";
    for (char** i = PHYSFS_getSearchPath(); *i != NULL; i++) {
        LOG_INFO << *i << "\n";
    }
}

bool ResourceManager::addResourceBundle(const string& bundle, const string& mountPoint) {
    return PHYSFS_mount(bundle.c_str(), mountPoint.c_str(), 1) != 0;
}

void ResourceManager::setWriteDirectory(const string& writeDir) {
    string path = getWritableDataPath(writeDir);

    struct stat statInfo;
    if ((stat(path.c_str(), &statInfo) == -1)) {
#ifndef _MSC_VER
        mkdir(path.c_str(), 0777);
#else
        _mkdir(path.c_str());
#endif
    }
    int result = PHYSFS_setWriteDir(path.c_str());
    if (result == 0) {
        LOG_INFO << "PHYSFS_setWriteDir failed\n";
    }
    PHYSFS_mount(path.c_str(), "/", 0);
    dump();
}

bool ResourceManager::hasResource(const string& name) {
    return PHYSFS_exists(name.c_str());
}

void ResourceManager::getFiles(const string& dirName, list<string>& results) {
    char** files = PHYSFS_enumerateFiles(dirName.c_str());
    if (files) {
        for (char** i = files; *i != NULL; i++) {
            results.push_back(string(*i));
        }

        PHYSFS_freeList(files);
    }
}

int ResourceManager::getResourceSize(const string& name) {
    PHYSFS_File* physFile = PHYSFS_openRead(name.c_str());
    if (!physFile) {
        return -1;
    }

    int size = (int)PHYSFS_fileLength(physFile);
    PHYSFS_close(physFile);
    return size;
}

ziStream* ResourceManager::getInputStream(const string& name) {
    if (!hasResource(name)) {
        return 0;
    }
    return new ziStream(name);
}
