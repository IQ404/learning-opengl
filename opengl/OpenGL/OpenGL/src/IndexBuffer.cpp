#include "IndexBuffer.h"
#include "DebugTools.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)	// count is the number of indices; bind after initialization
    : m_IndicesCount{ count }
{
    ASSERT_DebugBreak_MSVC(sizeof(unsigned int) == sizeof(GLuint));

    GLCall(glGenBuffers(1, &m_IBOID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesCount * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_IBOID));
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOID));
}

void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned int IndexBuffer::GetIndicesCount() const
{
    return m_IndicesCount;
}
