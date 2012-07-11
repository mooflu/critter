#pragma once
// Description:
//   Basic enemy controller. Controls enemy movement along paths.
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

#include <BaseEnemy.hpp>

class Model;
class SimpleEnemy: public BaseEnemy
{
public:
    SimpleEnemy( LEnemy *le);
    virtual ~SimpleEnemy();

    virtual void init( ParticleInfo *p);
    virtual bool update( ParticleInfo *p);
    virtual void draw( ParticleInfo *p);
    virtual void hit( ParticleInfo *p, int /*damage*/, int /*radIndex*/);

private:
    Model *_model;
};
