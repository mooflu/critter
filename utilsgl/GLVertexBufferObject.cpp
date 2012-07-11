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

#include "Trace.hpp"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLVBO::GLVBO():
    _vboBuffer(0),
    _iboBuffer(0),
    _hasNormal(false),
    _hasColor(false),
    _hasTexture(false)
{
}

GLVBO::~GLVBO()
{
    reset();
}

void GLVBO::reset()
{
    if( _vboBuffer)
    {
        glDeleteBuffers(1, &_vboBuffer);
        _vboBuffer = 0;
    }        
    if( _iboBuffer)
    {
        glDeleteBuffers(1, &_iboBuffer);
        _iboBuffer = 0;
    }        
}

void GLVBO::init( const std::vector<vec3f> &verts,
           const std::vector<vec3f> &norms,
           const std::vector<vec2f> &texels,
           const std::vector<vec4f> &colors )
{
    reset();
    glGenBuffers( 1, &_vboBuffer);
    glGenBuffers( 1, &_iboBuffer);

    _vertexCount = verts.size();

    if( norms.size() != 0)
    {
        _hasNormal = true; 
        if( norms.size() != verts.size())
        {
            LOG_WARNING << "VBO: mismatching norm vector size\n";
            _hasNormal = false;
        }
    }
    if( texels.size() != 0)
    {
        _hasTexture = true; 
        if( texels.size() != verts.size())
        {
            LOG_WARNING << "VBO: mismatching texels vector size\n";
            _hasTexture = false; 
        }
    }
    if( colors.size() != 0)
    {
        _hasColor = true; 
        if( colors.size() != verts.size())
        {
            LOG_WARNING << "VBO: mismatching colors vector size\n";
            _hasColor = false;
        }
    }

    _vertsOffset = 0;
    _normsOffset = sizeof(vec3f);
    _texelsOffset = _normsOffset + sizeof(vec3f);
    _colorsOffset = _texelsOffset + sizeof(vec2f);

    GLsizeiptr totalVBOSize = sizeof(VertexInfo) * _vertexCount;
    char *vboRawBuffer = new char[totalVBOSize];
    VertexInfo *vi = (VertexInfo*)& vboRawBuffer[0];

    GLsizeiptr totalIBOSize = sizeof(GLushort) * _vertexCount;
    char *iboRawBuffer = new char[totalIBOSize];
    GLushort *indices = (GLushort *) &iboRawBuffer[0];

    for( unsigned int i=0; i<verts.size(); i++)
    {
        indices[i] = i;
        vi[i].v = verts[i];

        if( _hasNormal)
        {
            vi[i].n = norms[i];
        }
        if( _hasTexture)
        {
            vi[i].t = texels[i];
        }
        if( _hasColor)
        {
            vi[i].c = colors[i];
        }
    }

    bind();

    glBufferData(GL_ARRAY_BUFFER, totalVBOSize, vboRawBuffer, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIBOSize, iboRawBuffer, GL_STATIC_DRAW);        

    unbind();

    delete[] vboRawBuffer;
    delete[] iboRawBuffer;
}

void GLVBO::draw()
{
    if( !_vboBuffer) return;
    
    bind();

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(VertexInfo), BUFFER_OFFSET(_vertsOffset));

    if( _hasNormal)
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(VertexInfo), BUFFER_OFFSET(_normsOffset));
    }
    if( _hasTexture)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(VertexInfo), BUFFER_OFFSET(_texelsOffset));
    }
    if( _hasColor)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, sizeof(VertexInfo), BUFFER_OFFSET(_colorsOffset));
    }

    glDrawElements(GL_TRIANGLES, _vertexCount, GL_UNSIGNED_SHORT, (void*)0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);            
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);            
    glDisableClientState(GL_COLOR_ARRAY);            

    unbind();
}

void GLVBO::DrawQuad( int type, const vec3f &p1, const vec3f &p2, const vec3f &p3, const vec3f &p4)
{
    GLfloat verts[] = {
        p1.x(), p1.y(), p1.z(),
        p2.x(), p2.y(), p2.z(),
        p3.x(), p3.y(), p3.z(),
        p4.x(), p4.y(), p4.z()
    };
    glVertexPointer(3, GL_FLOAT, 0, verts);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(type, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void GLVBO::DrawQuad( int type, const vec3f v[4])
{
    glVertexPointer(3, GL_FLOAT, 0, v[0].array);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(type, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void GLVBO::DrawQuadWithNormals( int type, const vec3f v[4], const vec3f n[4])
{
    glVertexPointer(3, GL_FLOAT, 0, v[0].array);        
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, n[0].array);        
    glEnableClientState(GL_NORMAL_ARRAY);
    
    glDrawArrays(type, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);            
}
