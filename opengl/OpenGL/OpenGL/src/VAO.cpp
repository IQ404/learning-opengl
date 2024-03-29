#include "VAO.h"
#include "DebugTools.h"

VAO::VAO()
{
	GLCall(glGenVertexArrays(1, &m_VAOID));
	//GLCall(glBindVertexArray(m_VAOID));
}

VAO::~VAO()
{
	GLCall(glDeleteVertexArrays(1, &m_VAOID));
}

void VAO::Bind() const
{
	GLCall(glBindVertexArray(m_VAOID));
}

void VAO::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

void VAO::LinkVertexBuffer(const VBO& vbo, const VBOLayout& layout)
{
	Bind();

	vbo.Bind();
	
	const auto& attributes = layout.GetAttributes();

	unsigned int offset = 0;

	for (unsigned int i = 0; i < attributes.size(); i++)
	{
		const auto& attribute = attributes[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, attribute.components_count, attribute.opengl_type, attribute.normalized, layout.GetStride(), (const void*)offset));  // this line links the vbo to the vao at slot index i
		offset += attribute.components_count * VBOAttribute::SizeOfOpenGLType(attribute.opengl_type);
	}
}

