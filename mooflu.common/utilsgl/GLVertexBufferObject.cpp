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
#include "GLVertexBufferObject.hpp"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "gl3/ProgramManager.hpp"
#include "gl3/Program.hpp"
#include "gl3/Buffer.hpp"
#include "gl3/VertexArray.hpp"
#include "gl3/MatrixStack.hpp"

#include "Trace.hpp"

GLVBO::GLVBO() :
    _hasColor(false),
    _hasTexture(false),
    _vertexCount(0),
    _vao(0),
    _vIndexBuf(0),
    _vertBuf(0),
    _texBuf(0),
    _colorBuf(0),
    _color(-1, -1, -1, 1) {}

GLVBO::~GLVBO() {
    reset();
}

void GLVBO::setColor(const vec4f& color) {
    _color = color;
}

void GLVBO::setColor(float r, float g, float b, float a) {
    _color = vec4f(r, g, b, a);
}

void GLVBO::reset() {
    delete _vIndexBuf;
    _vIndexBuf = 0;
    delete _vertBuf;
    _vertBuf = 0;
    delete _texBuf;
    _texBuf = 0;
    delete _colorBuf;
    _colorBuf = 0;
    delete _vao;
    _vao = 0;
}

void GLVBO::init(std::vector<vec4f>& verts, std::vector<vec2f>& texels, std::vector<vec4f>& colors) {
    _vertexCount = (unsigned int)verts.size();

    if (texels.size() != 0) {
        _hasTexture = true;
        if (texels.size() != verts.size()) {
            LOG_WARNING << "VBO: mismatching texels vector size\n";
            _hasTexture = false;
        }
    }
    if (colors.size() != 0) {
        _hasColor = true;
        if (colors.size() != verts.size()) {
            LOG_WARNING << "VBO: mismatching colors vector size\n";
            _hasColor = false;
        }
    }

    _vertBuf = new Buffer();
    _texBuf = new Buffer();
    _colorBuf = new Buffer();
    _vIndexBuf = new Buffer();

    Program* prog = ProgramManagerS::instance()->getProgram("texture");
    prog->use();

    _vao = new VertexArray();
    _vao->bind();

    GLint vertLoc = 0;
    glEnableVertexAttribArray(vertLoc);
    _vertBuf->bind(GL_ARRAY_BUFFER);
    _vertBuf->setData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec4f), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

    GLint withTexture = glGetUniformLocation(prog->id(), "withTexture");
    GLint textureUnit = glGetUniformLocation(prog->id(), "textureUnit");
    GLint texLoc = 1;
    glUniform1i(textureUnit, 0);
    if (_hasTexture) {
        glEnableVertexAttribArray(texLoc);
        _texBuf->bind(GL_ARRAY_BUFFER);
        _texBuf->setData(GL_ARRAY_BUFFER, texels.size() * sizeof(vec2f), texels.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glUniform1i(withTexture, 1);
    } else {
        //glDisableVertexAttribArray( texLoc );
        glUniform1i(withTexture, 0);
    }

    GLint colorLoc = 2;
    if (_hasColor) {
        glEnableVertexAttribArray(colorLoc);
        _colorBuf->bind(GL_ARRAY_BUFFER);
        _colorBuf->setData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec4f), colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    } else {
        //glDisableVertexAttribArray( colorLoc );
    }

    GLuint* vi = new GLuint[verts.size()];
    for (GLuint i = 0; i < (GLuint)verts.size(); i++) {
        vi[i] = i;
    }
    _vIndexBuf->bind(GL_ELEMENT_ARRAY_BUFFER);
    _vIndexBuf->setData(GL_ELEMENT_ARRAY_BUFFER, verts.size() * sizeof(GLuint), vi, GL_STATIC_DRAW);
    _vao->unbind();
    delete[] vi;
}

void GLVBO::draw(GLenum mode) {
    glm::mat4& modelview = MatrixStack::model.top();
    glm::mat4& projection = MatrixStack::projection.top();

    Program* prog = ProgramManagerS::instance()->getProgram("texture");
    prog->use();  //needed to set uniforms
    GLint modelViewMatrixLoc = glGetUniformLocation(prog->id(), "modelViewMatrix");
    glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection * modelview));
    GLint color = glGetUniformLocation(prog->id(), "aColor");
    glUniform4fv(color, 1, _color.array);

    _vao->bind();
    glDrawElements(mode, _vertexCount, GL_UNSIGNED_INT, NULL);
    _vao->unbind();
}

void GLVBO::DrawQuad(const vec4f& p1, const vec4f& p2, const vec4f& p3, const vec4f& p4) {
    std::vector<vec4f> verts = {p1, p2, p3, p4};
    std::vector<vec4f> colors;
    std::vector<vec2f> texels;
    init(verts, texels, colors);
    draw(GL_TRIANGLE_FAN);
}

void GLVBO::DrawQuad(const vec4f v[4]) {
    std::vector<vec4f> verts = {v[0], v[1], v[2], v[3]};
    std::vector<vec4f> colors;
    std::vector<vec2f> texels;
    init(verts, texels, colors);
    draw(GL_TRIANGLE_FAN);
}

void GLVBO::DrawTexQuad(const vec4f v[4], const vec2f t[4]) {
    std::vector<vec4f> verts = {v[0], v[1], v[2], v[3]};
    std::vector<vec4f> colors;
    std::vector<vec2f> texels = {t[0], t[1], t[2], t[3]};
    init(verts, texels, colors);
    draw(GL_TRIANGLE_FAN);
}

void GLVBO::DrawColorQuad(const vec4f v[4], const vec4f c[4]) {
    std::vector<vec4f> verts = { v[0], v[1], v[2], v[3] };
    std::vector<vec4f> colors = { c[0], c[1], c[2], c[3] };
    std::vector<vec2f> texels;
    init(verts, texels, colors);
    draw(GL_TRIANGLE_FAN);
}

void GLVBO::DrawPoints(GLfloat* v, int numVerts) {
    std::vector<vec4f> verts;
    std::vector<vec4f> colors;
    std::vector<vec2f> texels;
    for (size_t i = 0; i < (numVerts*3); i += 3) {
        verts.push_back(vec4f(v[i], v[i + 1], v[i + 2], 1));
    }

    init(verts, texels, colors);
    draw(GL_POINTS);
}

void GLVBO::DrawColorPoints(GLfloat* v, int numVerts, GLfloat* c, int numColors) {
    std::vector<vec4f> verts;
    std::vector<vec4f> colors;
    std::vector<vec2f> texels;
    for (size_t i = 0; i < (numVerts * 3); i += 3) {
        verts.push_back(vec4f(v[i], v[i + 1], v[i + 2], 1));
    }

    for (size_t i = 0; i < (numColors * 2); i += 2) {
        colors.push_back(vec4f(c[i], c[i + 1], c[i + 2], 1));
    }

    init(verts, texels, colors);
    draw(GL_POINTS);
}
