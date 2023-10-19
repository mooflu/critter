#pragma once
// Description:
//   GL VBO
//
// Copyright (C) 2010 Frank Becker
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

#include "vmmlib/vector.hpp"
using namespace vmml;

#include <vector>

class Buffer;
class VertexArray;

class GLVBO {
public:
    GLVBO();
    ~GLVBO();

    void reset();
    void init(std::vector<vec4f>& verts, std::vector<vec2f>& texels, std::vector<vec4f>& colors);

    void setColor(const vec4f& color);
    void setColor(float r, float g, float b, float a);

    void draw(GLenum mode);

    void DrawQuad(const vec4f& p1, const vec4f& p2, const vec4f& p3, const vec4f& p4);
    void DrawQuad(const vec4f v[4]);
    void DrawTexQuad(const vec4f v[4], const vec2f t[4]);

    void DrawPoints(GLfloat* verts, int numVerts);
    void DrawColorPoints(GLfloat* verts, int numVerts, GLfloat* colors, int numColors);

private:
    bool _hasColor;
    bool _hasTexture;

    unsigned int _vertexCount;

    VertexArray* _vao;
    Buffer* _vIndexBuf;
    Buffer* _vertBuf;
    Buffer* _texBuf;
    Buffer* _colorBuf;

    vec4f _color;
};
