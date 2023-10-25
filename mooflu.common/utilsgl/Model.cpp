// Description:
//   Simple 3D Model loader.
//
// Copyright (C) 2011 Frank Becker
//
#include "Model.hpp"

#include "Trace.hpp"
#include "Tokenizer.hpp"
#include "ResourceManager.hpp"

#include "gl3/ProgramManager.hpp"
#include "gl3/Program.hpp"
#include "gl3/Buffer.hpp"
#include "gl3/VertexArray.hpp"
#include "gl3/MatrixStack.hpp"

#include "glm/ext.hpp"

#include <memory>
#include <vector>
using namespace std;

#ifdef IPHONE
float Model::MODEL_SCALE = 1.3f;
#else
float Model::MODEL_SCALE = 1.0f;
#endif

Model::Model(void) :
    _numVerts(0),
    _numColors(0),
    _numFaces(0),
    _verts(0),
    _norms(0),
    _colors(0),
    _faces(0),
    _offset(0, 0, 0),
    _fixNormals(false),
    _vao(0),
    _vIndexBuf(0),
    _vertBuf(0),
    _normBuf(0),
    _colorBuf(0),
    _numTriangles(0),
    _color(-1, -1, -1, 1) {
    XTRACE();
}

Model::~Model() {
    XTRACE();
    reset();
    delete[] _faces;
}

void Model::setColor(const vec4f& color) {
    _color = color;
}

void Model::setColor(float r, float g, float b, float a) {
    _color = vec4f(r, g, b, a);
}

//Load model from file
bool Model::load(const char* filename) {
    XTRACE();
    //HACK to fixup models with bad normals or models that require
    //GL_LIGHT_MODEL_TWO_SIDE, but doesn't work on IPHONE
    string fileName = filename;
    size_t len = fileName.length();
    if (fileName[len - 7] == 'X') {
        _fixNormals = true;
        fileName.replace(fileName.end() - 7, fileName.end(), ".model");
    }

    if (!ResourceManagerS::instance()->hasResource(fileName)) {
        LOG_ERROR << "Unable to open: [" << fileName << "]" << endl;
        return false;
    }
    std::shared_ptr<ziStream> infilePtr(ResourceManagerS::instance()->getInputStream(fileName));
    ziStream& infile = *infilePtr;

    LOG_INFO << "  Model " << fileName << endl;

    vec3f scale;
    scale.x() = scale.y() = scale.z() = 1.0;

    string line;
    int linecount = 0;
    while (!getline(infile, line).eof()) {
        linecount++;

        //explicitly skip comments
        if (line[0] == '#') {
            continue;
        }
        Tokenizer t(line);
        string token = t.next();
        if (token == "Name") {
            _name = t.next();
            //            LOG_INFO << "Name = [" << _name << "]" << endl;
        } else if (token == "Scale") {
            scale.x() = (float)atof(t.next().c_str()) * MODEL_SCALE;
            scale.y() = (float)atof(t.next().c_str()) * MODEL_SCALE;
            scale.z() = (float)atof(t.next().c_str()) * MODEL_SCALE;
#if 0
            LOG_INFO << "Scale = ["
                << scale.x() << ","
                << scale.y() << ","
                << scale.z()
                << "]" << endl;
#endif
        } else if (token == "Colors") {
            _numColors = atoi(t.next().c_str());
            if (!readColors(infile, linecount)) {
                LOG_ERROR << "Error reading colors:"
                          << " line:" << linecount << endl;
                return false;
            }
        } else if (token == "Vertices") {
            int numVerts = atoi(t.next().c_str());
            verifyAndAssign(numVerts, _numVerts);
            if (!readVertices(infile, scale, linecount)) {
                LOG_ERROR << "Error reading vertices:"
                          << " line:" << linecount << endl;
                return false;
            }
        } else if (token == "Normals") {
            int numNorms = atoi(t.next().c_str());
            verifyAndAssign(numNorms, _numVerts);
            if (!readNormals(infile, linecount)) {
                LOG_ERROR << "Error reading normals:"
                          << " line:" << linecount << endl;
                return false;
            }
        } else if (token == "Faces") {
            _numFaces = atoi(t.next().c_str());
            _faces = new FaceInfo[_numFaces];
            if (!readFaces(infile, linecount)) {
                LOG_ERROR << "Error reading faces:"
                          << " line:" << linecount << endl;
                return false;
            }
        } else if (token == "Offset") {
            _offset.x() = (float)atof(t.next().c_str()) * MODEL_SCALE;
            _offset.y() = (float)atof(t.next().c_str()) * MODEL_SCALE;
            _offset.z() = (float)atof(t.next().c_str()) * MODEL_SCALE;
#if 0
            LOG_INFO << "Offset = ["
                << _offset.x() << ","
                << _offset.y() << ","
                << _offset.z()
                << "]" << endl;
#endif
        } else {
            LOG_ERROR << "Syntax error: [" << token << "] line:" << linecount << endl;
            return false;
        }
    }

    prepareModel();

    return true;
}

//read vertices section
bool Model::readVertices(ziStream& infile, const vec3f& scale, int& linecount) {
    XTRACE();
    string line;
    for (int i = 0; i < _numVerts; i++) {
        if (getline(infile, line).eof()) {
            return false;
        }
        linecount++;

        Tokenizer t(line);

        float x = (float)atof(t.next().c_str());
        float y = (float)atof(t.next().c_str());
        float z = (float)atof(t.next().c_str());

        _verts.push_back(vec3f(x * scale.x(), y * scale.y(), z * scale.z()));

        if (t.tokensReturned() != 3) {
            return false;
        }

        if (i == 0) {
            _min = _verts[i];
            _max = _verts[i];
        } else {
            if (_verts[i].x() < _min.x()) {
                _min.x() = _verts[i].x();
            }
            if (_verts[i].y() < _min.y()) {
                _min.y() = _verts[i].y();
            }
            if (_verts[i].z() < _min.z()) {
                _min.z() = _verts[i].z();
            }

            if (_verts[i].x() > _max.x()) {
                _max.x() = _verts[i].x();
            }
            if (_verts[i].y() > _max.y()) {
                _max.y() = _verts[i].y();
            }
            if (_verts[i].z() > _max.z()) {
                _max.z() = _verts[i].z();
            }
        }
    }

    return true;
}

//read normals section
bool Model::readNormals(ziStream& infile, int& linecount) {
    XTRACE();
    string line;
    for (int i = 0; i < _numVerts; i++) {
        if (getline(infile, line).eof()) {
            return false;
        }
        linecount++;

        Tokenizer t(line);

        float x = (float)atof(t.next().c_str());
        float y = (float)atof(t.next().c_str());
        float z = (float)atof(t.next().c_str());

        vec3f norm(x, y, z);
        //norm.normalize();

        _norms.push_back(norm);

        if (_fixNormals && (_norms[i].z() < 0)) {
            _norms[i].x() = -_norms[i].x();
            _norms[i].y() = -_norms[i].y();
            _norms[i].z() = -_norms[i].z();
        }

        if (t.tokensReturned() != 3) {
            return false;
        }
    }

    return true;
}

//read faces section
bool Model::readFaces(ziStream& infile, int& linecount) {
    XTRACE();
    string line;
    for (int i = 0; i < _numFaces; i++) {
        if (getline(infile, line).eof()) {
            return false;
        }
        linecount++;

        Tokenizer t(line);

        _faces[i].v1 = atoi(t.next().c_str());
        _faces[i].v2 = atoi(t.next().c_str());
        _faces[i].v3 = atoi(t.next().c_str());
        _faces[i].v4 = atoi(t.next().c_str());
        _faces[i].smooth = (atoi(t.next().c_str()) == 1);
        _faces[i].color = atoi(t.next().c_str());

        if (t.tokensReturned() != 6) {
            return false;
        }
    }

    return true;
}

//read colors section
bool Model::readColors(ziStream& infile, int& linecount) {
    XTRACE();
    string line;
    for (int i = 0; i < _numColors; i++) {
        if (getline(infile, line).eof()) {
            return false;
        }
        linecount++;

        Tokenizer t(line);
        float r = (float)atof(t.next().c_str());
        float g = (float)atof(t.next().c_str());
        float b = (float)atof(t.next().c_str());

        _colors.push_back(vec4f(r, g, b, 1.0f));

        if (t.tokensReturned() != 3) {
            return false;
        }
    }

    return true;
}

//hum
void Model::verifyAndAssign(const int newVert, int& currVertVal) {
    XTRACE();
    if ((currVertVal != 0) && (newVert != currVertVal)) {
        LOG_ERROR << "Vertex count inconsistency!" << endl;
    } else {
        currVertVal = newVert;
    }
}

void Model::reset(void) {
    _numTriangles = 0;
    delete _vao;
    _vao = 0;
    delete _vIndexBuf;
    _vIndexBuf = 0;
    delete _vertBuf;
    _vertBuf = 0;
    delete _normBuf;
    _normBuf = 0;
    delete _colorBuf;
    _colorBuf = 0;
}

//re-load model
void Model::reload(void) {
    XTRACE();
    prepareModel();
}

void Model::addTriangle(const vec4f& color, const vec4f& avgNormal, bool hasColor, int v1, int v2, int v3, bool smooth,
                        std::vector<vec3f>& verts, std::vector<vec3f>& norms, std::vector<vec4f>& colors) {
    int f[3];
    f[0] = v1;
    f[1] = v2;
    f[2] = v3;

    for (int i = 0; i < 3; i++) {
        if (hasColor) {
            colors.push_back(color);
        } else {
            colors.push_back(vec4f(1.0, 1.0, 1.0, 1.0));
        }

        if (smooth) {
            norms.push_back(_norms[f[i]]);
        } else {
            norms.push_back(avgNormal);
        }

        verts.push_back(_verts[f[i]]);
    }
    _numTriangles += 1;
}

void Model::draw() {
    Program* prog = ProgramManagerS::instance()->getProgram("lighting");
    prog->use();

    GLint modelLoc = glGetUniformLocation(prog->id(), "model");
    glm::mat4& modelview = MatrixStack::model.top();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelview));

    GLint objectColorLoc = glGetUniformLocation(prog->id(), "objectColor");
    glUniform4fv(objectColorLoc, 1, _color.array);

    _vao->bind();
    glDrawElements(GL_TRIANGLES, _numTriangles * 3, GL_UNSIGNED_INT, NULL);
    _vao->unbind();
}

void Model::prepareModel(void) {
    _vertBuf = new Buffer();
    _normBuf = new Buffer();
    _colorBuf = new Buffer();
    _vIndexBuf = new Buffer();

    Program* prog = ProgramManagerS::instance()->getProgram("lighting");
    prog->use();

    _vao = new VertexArray();
    _vao->bind();

    GLint vertLoc = 0;
    glEnableVertexAttribArray(vertLoc);
    _vertBuf->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint normLoc = 1;
    glEnableVertexAttribArray(normLoc);
    _normBuf->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint colorLoc = 2;
    glEnableVertexAttribArray(colorLoc);
    _colorBuf->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

    _vIndexBuf->bind(GL_ELEMENT_ARRAY_BUFFER);

    std::vector<vec3f> verts;
    std::vector<vec3f> norms;
    std::vector<vec2f> texels;
    std::vector<vec4f> colors;

    vec3f color(1.0, 0.0, 0.0);
    for (int i = 0; i < _numFaces; i++) {
        if (_numColors) {
            color = _colors[_faces[i].color];
        }

        vec3f avgNormal = _norms[_faces[i].v1] + _norms[_faces[i].v2] + _norms[_faces[i].v3];
        if (_faces[i].v4 != 0) {
            avgNormal += _norms[_faces[i].v4];
        }
        avgNormal.normalize();

        addTriangle(color, avgNormal, _numColors > 0, _faces[i].v1, _faces[i].v2, _faces[i].v3, _faces[i].smooth, verts,
                    norms, colors);
        if (_faces[i].v4 != 0)  //quad
        {
            addTriangle(color, avgNormal, _numColors > 0, _faces[i].v3, _faces[i].v4, _faces[i].v1, _faces[i].smooth,
                        verts, norms, colors);
        }
    }
    GLuint* vi = new GLuint[verts.size()];
    for (GLuint i = 0; i < verts.size(); i++) {
        vi[i] = i;
    }
    _vIndexBuf->bind(GL_ELEMENT_ARRAY_BUFFER);
    _vIndexBuf->setData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLuint), vi, GL_STATIC_DRAW);

    _vertBuf->bind(GL_ARRAY_BUFFER);
    _vertBuf->setData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec3f), verts.data(), GL_STATIC_DRAW);

    _normBuf->bind(GL_ARRAY_BUFFER);
    _normBuf->setData(GL_ARRAY_BUFFER, norms.size() * sizeof(vec3f), norms.data(), GL_STATIC_DRAW);

    _colorBuf->bind(GL_ARRAY_BUFFER);
    _colorBuf->setData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec4f), colors.data(), GL_STATIC_DRAW);

    _vao->unbind();
    delete[] vi;
}
