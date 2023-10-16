#pragma once

#include <GL/glew.h>

class Buffer {
public:
    Buffer();
    virtual ~Buffer();

    GLuint id() const;

    void bind(GLenum target) const;
    static void unbind(GLenum target);
    static void unbind(GLenum target, GLuint index);

    void setData(GLenum target, size_t size, void* data, GLenum usage);

private:
    GLuint _id;
};