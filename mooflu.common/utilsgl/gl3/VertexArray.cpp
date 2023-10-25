#include "VertexArray.hpp"

#include "Buffer.hpp"

VertexArray::VertexArray() :
    _id(0) {
    glGenVertexArrays(1, &_id);
}

VertexArray::~VertexArray() {
    unbind();
    glDeleteVertexArrays(1, &_id);
}

GLuint VertexArray::id() const {
    return _id;
}

void VertexArray::bind() const {
    glBindVertexArray(id());
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::bindElementBuffer(const Buffer* buffer) {
    bind();
    if (buffer) {
        buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
    } else {
        Buffer::unbind(GL_ELEMENT_ARRAY_BUFFER);
    }
}

void VertexArray::enable(GLint attributeIndex) {
    bind();
    glEnableVertexAttribArray(attributeIndex);
}

void VertexArray::disable(GLint attributeIndex) {
    bind();
    glDisableVertexAttribArray(attributeIndex);
}

/*
Create VBOs/IBO
    glGenBuffers( 1, &vertexVBO );
    glBindBuffer( GL_ARRAY_BUFFER, vertexVBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(Vert2D), quadPos, GL_STATIC_DRAW );

    glGenBuffers( 1, &color1VBO );
    glBindBuffer( GL_ARRAY_BUFFER, color1VBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(Color), quadColorRGBY, GL_STATIC_DRAW );

    glGenBuffers( 1, &color2VBO );
    glBindBuffer( GL_ARRAY_BUFFER, color2VBO );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(Color), quadColorCYMW, GL_STATIC_DRAW );

    glGenBuffers( 1, &indexBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indices, GL_STATIC_DRAW );

VAO setup:
    vertexAttributeLocation = glGetAttribLocation( programID, "vertexPos2D" );
    color1AttributeLocation = glGetAttribLocation( programID, "color1" );
    color2AttributeLocation = glGetAttribLocation( programID, "color2" );

    glGenVertexArrays( 1, &id );
    glBindVertexArray( id );

    glEnableVertexAttribArray( vertexAttributeLocation )
    glBindBuffer( GL_ARRAY_BUFFER, vertexVBO );
    glVertexAttribPointer( vertexAttributeLocation, 2, GL_FLOAT, GL_FALSE, stride, data );

    glEnableVertexAttribArray( color1AttributeLocation )
    glBindBuffer( GL_ARRAY_BUFFER, color1VBO );
    glVertexAttribPointer( color1AttributeLocation, 4, GL_FLOAT, GL_FALSE, stride, data );

    glEnableVertexAttribArray( color2AttributeLocation )
    glBindBuffer( GL_ARRAY_BUFFER, color2VBO );
    glVertexAttribPointer( color2AttributeLocation, 4, GL_FLOAT, GL_FALSE, stride, data );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBO );
    glBindVertexArray( 0 );


VAO usage:
    set/update model view
    ...

    glBindVertexArray( id );
    glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );
*/