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

#include "GLee.h"

#include "vmmlib/vector.hpp"
using namespace vmml;

#include <vector>

class GLVBO
{
public:
    struct VertexInfo
    {
        vec3f v; //offset 0
        vec3f n; //offset 12
        vec2f t; //offset 24
        vec4f c; //offset 32
        float pad[4]; //pad to size 64
    }; //size 64
    
    GLVBO();
    ~GLVBO();

    void reset();
    void init( const std::vector<vec3f> &verts,
               const std::vector<vec3f> &norms,
               const std::vector<vec2f> &texels,
               const std::vector<vec4f> &colors );
    void draw();
    
    static void DrawQuad( int type, const vec3f &p1, const vec3f &p2, const vec3f &p3, const vec3f &p4);
    static void DrawQuad( int type, const vec3f v[4]);
    static void DrawQuadWithNormals( int type, const vec3f v[4], const vec3f n[4]);
    
private:
    void bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vboBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboBuffer);
    }

    void unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GLuint _vboBuffer;
    GLuint _iboBuffer;

    bool _hasNormal;
    bool _hasColor;
    bool _hasTexture;

    int _vertsOffset;
    int _normsOffset;
    int _texelsOffset;
    int _colorsOffset;

    int _vertexCount;
};
