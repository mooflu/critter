#include "Program.hpp"

#include "Shader.hpp"

#include "Trace.hpp"

#include <string>

Program::Program() :
    _id(0),
    _linked(false),
    _dirty(true) {
    _id = glCreateProgram();
}

Program::~Program() {
    LOG_INFO << "Deleting program " << _id << "\n";
    while (!_shaders.empty()) {
        auto shader = *_shaders.begin();
        delete shader;
    }

    glDeleteProgram(_id);
}

GLuint Program::id() const {
    return _id;
}

void Program::use() const {
    checkDirty();

    if (!isLinked()) {
        return;
    }

    glUseProgram(id());
}

void Program::release() {
    glUseProgram(0);
}

bool Program::isUsed() const {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    return currentProgram != 0 && static_cast<GLuint>(currentProgram) == id();
}

void Program::attach(Shader* shader) {
    glAttachShader(id(), shader->id());

    _shaders.insert(shader);
    shader->_programs.insert(this);

    invalidate();
}

void Program::detach(Shader* shader) {
    glDetachShader(id(), shader->id());

    _shaders.erase(shader);
    shader->_programs.erase(this);

    invalidate();
}

void Program::checkDirty() const {
    if (_dirty) {
        link();
    }
}

void Program::link() const {
    _linked = false;

    compileAttachedShaders();

    glLinkProgram(id());

    _linked = get(GL_LINK_STATUS) == GL_TRUE;
    _dirty = false;

    if (!_linked) {
        char logBuf[1024];
        int len;
        glGetProgramInfoLog(id(), sizeof(logBuf), &len, logBuf);

        LOG_ERROR << "Failed to link shader program:" << logBuf << "\n";
    }
#if 0
    updateUniforms();
    updateUniformBlockBindings();
#endif
}

bool Program::isLinked() const {
    return _linked;
}

bool Program::compileAttachedShaders() const {
    for (Shader* shader : _shaders) {
        if (shader->isCompiled()) {
            continue;
        }

        shader->compile();

        if (!shader->isCompiled()) {
            return false;
        }
    }

    return true;
}

GLint Program::get(const GLenum pname) const {
    GLint value = 0;
    glGetProgramiv(id(), pname, &value);

    return value;
}

void Program::validate() {
    glValidateProgram(id());
}

bool Program::isValid() const {
    return get(GL_VALIDATE_STATUS) == 1;
}

void Program::invalidate() const {
    _dirty = true;
}

void Program::setParameter(GLenum pname, GLint value) const {
    glProgramParameteri(id(), pname, value);
}

void Program::setParameter(GLenum pname, GLboolean value) const {
    glProgramParameteri(id(), pname, value == GL_TRUE ? 1 : 0);
}

GLint Program::getAttributeLocation(const std::string& name) const {
    checkDirty();

    if (!_linked) {
        return -1;
    }

    return glGetAttribLocation(id(), name.c_str());
}

GLint Program::getUniformLocation(const std::string& name) const {
    checkDirty();

    if (!_linked) {
        return -1;
    }

    return glGetUniformLocation(id(), name.c_str());
}

void Program::bindAttributeLocation(const GLuint index, const std::string& name) const {
    glBindAttribLocation(id(), index, name.c_str());
}

void Program::bindFragDataLocation(const GLuint index, const std::string& name) const {
    glBindFragDataLocation(id(), index, name.c_str());
}
