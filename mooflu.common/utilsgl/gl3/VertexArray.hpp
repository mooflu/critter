#pragma once

#include <GL/glew.h>

class Buffer;

class VertexArray {
public:
    VertexArray();
    virtual ~VertexArray();

    GLuint id() const;

    void bind() const;
    static void unbind();

    void bindElementBuffer(const Buffer* buffer);

    void enable(GLint attributeIndex);
    void disable(GLint attributeIndex);

private:
    GLuint _id;
};
