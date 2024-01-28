#include "VBO.h"
#include "DebugTools.h"

VBO::VBO(const void* data, unsigned int size)   // size in bytes; bind after initialization
{
    GLCall(glGenBuffers(1, &m_VBOID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBOID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VBO::~VBO()
{
    GLCall(glDeleteBuffers(1, &m_VBOID));
}

void VBO::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBOID));
}

void VBO::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
