#pragma once
// Description:
//   Simple 3D Model loader.
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

#include "zStream.hpp"
#include <GL/glew.h>

#include "vmmlib/vector.hpp"
using namespace vmml;

#include "Point.hpp"

class Buffer;
class VertexArray;

struct FaceInfo {
    int v1;
    int v2;
    int v3;
    int v4;
    int color;
    bool smooth;
};

class Model {
public:
    Model(void);
    ~Model();

    static float MODEL_SCALE;  //global scale factor (mostly for iPhone)

    //Load model from file
    bool load(const char* filename);
    //go draw
    void draw();
    //re-load model (e.g. after toggling fullscreen).
    void reload(void);
    void reset(void);

    //name of model
    const std::string getName(void) { return _name; }
#if 0
    void getBoundingBox( vec3f &min, vec3f &max)
    {
        min = _min;
        max = _max;
    }

    const vec3f & getOffset( void)
    {
        return _offset;
    }
#endif
    void getBoundingBox(vec3& min, vec3& max) {
        min = vec3(_min.x(), _min.y(), _min.z());
        max = vec3(_max.x(), _max.y(), _max.z());
    }

    const vec3 getOffset(void) { return vec3(_offset.x(), _offset.y(), _offset.z()); }

    void setColor(const vec4f& color);
    void setColor(float r, float g, float b, float a);

private:
    Model(const Model&);
    Model& operator=(const Model&);

    void prepareModel(void);

    void addTriangle(const vec4f& color, const vec4f& avgNormal, bool hasColor, int v1, int v2, int v3, bool smooth,
                     std::vector<vec3f>& verts, std::vector<vec3f>& norms, std::vector<vec4f>& colors);

    void verifyAndAssign(const int newVert, int& currVertVal);

    bool readColors(ziStream& infile, int& linecount);
    bool readFaces(ziStream& infile, int& linecount);
    bool readNormals(ziStream& infile, int& linecount);
    bool readVertices(ziStream& infile, const vec3f& scale, int& linecount);

    std::string _name;

    int _numVerts;
    int _numColors;
    int _numFaces;

    std::vector<vec3f> _verts;
    std::vector<vec3f> _norms;
    std::vector<vec4f> _colors;
    FaceInfo* _faces;

    vec3f _min;
    vec3f _max;
    vec3f _offset;

    bool _fixNormals;

    VertexArray* _vao;
    Buffer* _vIndexBuf;
    Buffer* _vertBuf;
    Buffer* _normBuf;
    Buffer* _colorBuf;
    int _numTriangles;

    vec4f _color;
};
