#ifndef VAO_H
#define VAO_H

#include "VBO.h"
#include "VBOLayout.h"

class VAO
{
	unsigned int m_RendererID;

public:

	VAO();
	
	~VAO();

	void Bind() const;

	void Unbind() const;

	void LinkVertexBuffer(const VBO& vbo, const VBOLayout& layout);
};

#endif // !VAO_H

