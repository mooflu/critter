#pragma once

#include <set>
#include <string>

#include <GL/glew.h>

class Shader;

class Program {
public:
    Program();
    virtual ~Program();

    GLuint id() const;

    void use() const;
    static void release();

    bool isUsed() const;

    void attach(Shader* shader);
    void detach(Shader* shader);

    void link() const;
    bool isLinked() const;
    bool compileAttachedShaders() const;

    GLint get(GLenum pname) const;

    void validate();
    bool isValid() const;
    void invalidate() const;

    void setParameter(GLenum pname, GLint value) const;
    void setParameter(GLenum pname, GLboolean value) const;

    GLint getAttributeLocation(const std::string& name) const;
    GLint getUniformLocation(const std::string& name) const;

    void bindAttributeLocation(GLuint index, const std::string& name) const;
    void bindFragDataLocation(GLuint index, const std::string& name) const;
#if 0
    GLint getFragDataLocation(const std::string & name) const;
    GLint getFragDataIndex(const std::string & name) const;
#endif

#if 0
    prog.use()
    glUniform4f( prog.getUniformLocation("variable"), r,g,b,a );

    template<typename T>
    void setUniform(const std::string & name, const T & value);
    template<typename T>
    void setUniform(GLint location, const T & value);
    template<typename T>
    Uniform<T> * getUniform(const std::string & name);
    template<typename T>
    const Uniform<T> * getUniform(const std::string & name) const;
    template<typename T>
    Uniform<T> * getUniform(GLint location);
    template<typename T>
    const Uniform<T> * getUniform(GLint location) const;
#endif

protected:
    void checkDirty() const;

    GLuint _id;
    std::set<Shader*> _shaders;

    mutable bool _linked;
    mutable bool _dirty;
};