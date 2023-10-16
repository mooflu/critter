#include "Buffer.hpp"

Buffer::Buffer() {
    glGenBuffers(1, &_id);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &_id);
}

GLuint Buffer::id() const {
    return _id;
}

void Buffer::bind(const GLenum target) const {
    glBindBuffer(target, id());
}

void Buffer::unbind(const GLenum target) {
    glBindBuffer(target, 0);
}

void Buffer::unbind(const GLenum target, const GLuint index) {
    glBindBufferBase(target, index, 0);
}

void Buffer::setData(GLenum target, size_t size, void* data, GLenum usage) {
    glBufferData(target, size, data, usage);
}
