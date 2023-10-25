#include "Shader.hpp"

#include "Trace.hpp"

#include "Program.hpp"

#include <vector>

Shader::Shader(const GLenum type) :
    _type(type),
    _compiled(false),
    _compilationFailed(false) {
    _id = glCreateShader(type);
}

Shader::Shader(const GLenum type, const std::string& source) :
    _type(type),
    _source(source),
    _compiled(false),
    _compilationFailed(false) {
    _id = glCreateShader(type);
}

Shader::~Shader() {
    LOG_INFO << "Deleting shader " << _id << "\n";
    while (!_programs.empty()) {
        (*_programs.begin())->detach(this);
    }

    _source = "";

    glDeleteShader(_id);
}

GLuint Shader::id() const {
    return _id;
}

GLenum Shader::type() const {
    return _type;
}

GLint Shader::get(GLenum pname) const {
    GLint value = 0;
    glGetShaderiv(id(), pname, &value);

    return value;
}

void Shader::setSource(const std::string& source) {
    if (source == _source) {
        return;
    }

    _source = source;
    invalidate();
}

bool Shader::compile() const {
    if (_compilationFailed) {
        return false;
    }

    const char* sourcePtr = _source.c_str();
    glShaderSource(id(), static_cast<GLint>(1), &sourcePtr, nullptr);
    glCompileShader(id());
    //glCompileShaderIncludeARB(id(), 0, nullptr, nullptr);

    _compiled = checkCompileStatus();
    _compilationFailed = !_compiled;

    return _compiled;
}

bool Shader::isCompiled() const {
    return _compiled;
}

void Shader::invalidate() {
    _compiled = false;
    _compilationFailed = false;
}

bool Shader::checkCompileStatus() const {
    GLboolean status = static_cast<GLboolean>(get(GL_COMPILE_STATUS));

    if (status == GL_FALSE) {
        logError();
        return false;
    }

    return true;
}

void Shader::logError() const {
    GLsizei length = get(GL_INFO_LOG_LENGTH);
    std::vector<char> log(length);

    glGetShaderInfoLog(id(), length, &length, log.data());

    std::string shaderLog(log.data(), length);

    LOG_ERROR << "Compiler error:" << std::endl << _source << std::endl << shaderLog;
}
