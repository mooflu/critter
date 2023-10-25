#pragma once
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

#include <string>

#include "hashMap.hpp"
#include "HashString.hpp"
#include "FindHash.hpp"

#include "Trace.hpp"
#include "Model.hpp"
#include "Singleton.hpp"

class ModelManager {
    friend class Singleton<ModelManager>;

public:
    Model* getModel(std::string modelName) {
        Model* model = findHash(modelName, _modelMap);
        if (!model) {
            model = load(modelName);
            if (!model) {
                LOG_ERROR << "Unable to find model " << modelName << "\n";
                return 0;
            }
            _modelMap[modelName] = model;
        }
        return model;
    }

    void reset(void);
    void reload(void);

private:
    ~ModelManager();
    ModelManager(void);
    ModelManager(const ModelManager&);
    ModelManager& operator=(const ModelManager&);

    Model* load(const std::string& model);

    hash_map<std::string, Model*, hash<std::string>, std::equal_to<std::string>> _modelMap;
};

typedef Singleton<ModelManager> ModelManagerS;
