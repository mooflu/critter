// Description:
//   Model Manager.
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
#include "ModelManager.hpp"

#include <sys/types.h>
#include "ResourceManager.hpp"

using namespace std;

ModelManager::ModelManager(void) {
    XTRACE();
}

ModelManager::~ModelManager() {
    XTRACE();

    hash_map<string, Model*, hash<string>>::const_iterator ci;
    for (ci = _modelMap.begin(); ci != _modelMap.end(); ci++) {
        delete ci->second;
    }

    _modelMap.clear();
}

void ModelManager::reset(void) {
    XTRACE();

    hash_map<string, Model*, hash<string>>::const_iterator ci;
    for (ci = _modelMap.begin(); ci != _modelMap.end(); ci++) {
        ci->second->reset();
    }
}

void ModelManager::reload(void) {
    XTRACE();

    hash_map<string, Model*, hash<string>>::const_iterator ci;
    for (ci = _modelMap.begin(); ci != _modelMap.end(); ci++) {
        ci->second->reload();
    }
}

Model* ModelManager::load(const string& modelName) {
    XTRACE();
    Model* model = new Model;

    string modelFile = modelName + ".model";

    if (!model->load(modelFile.c_str())) {
        LOG_ERROR << "Unable to load: " << modelFile << endl;
    }

    return model;
}
