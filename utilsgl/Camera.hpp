#pragma once
// Description:
//   Camera to enable 1st person shooter type mouselook.
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

#include <GL/glew.h>
#include "Trace.hpp"
#include "Direction.hpp"
#include "Singleton.hpp"

class Camera {
    friend class Singleton<Camera>;

public:
    void place(void);
    void billboard(void);
    void mouseLook(float dx, float dy);
    void update(void);
    void move(Direction::DirectionEnum dir, bool isDown);

private:
    ~Camera() { XTRACE(); }

    Camera(void) :
        _moveCam(false),
        _cameraMove(0.0),
        _cameraX(0.0),
        _cameraY(0.0),
        _cameraZ(0.0),
        _cameraYaw(0.0),
        _cameraPitch(0.0) {
        XTRACE();
    }

    Camera(const Camera&);
    Camera& operator=(const Camera&);

    void move(GLfloat stepSize, GLfloat angle);
    void clampCamera(void);

    bool _moveCam;
    GLfloat _cameraMove;
    GLfloat _cameraX;
    GLfloat _cameraY;
    GLfloat _cameraZ;
    GLfloat _cameraYaw;
    GLfloat _cameraPitch;
};

typedef Singleton<Camera> CameraS;
