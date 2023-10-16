#pragma once

#include <GL/glew.h>

#include <set>
#include <string>

class Program;

class Shader {
    friend class Program;

public:
    Shader(const GLenum type);
    Shader(const GLenum type, const std::string& source);
    virtual ~Shader();

    GLuint id() const;
    GLenum type() const;

    GLint get(GLenum pname) const;

    void setSource(const std::string& source);

    bool compile() const;
    bool isCompiled() const;
    void invalidate();

    bool checkCompileStatus() const;
    void logError() const;

protected:
    GLuint _id;
    GLenum _type;
    std::string _source;
    std::set<Program*> _programs;

    mutable bool _compiled;
    mutable bool _compilationFailed;
};
