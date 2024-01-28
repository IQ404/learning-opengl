#ifndef VAO_H
#define VAO_H

#include "VBO.h"
#include "VBOLayout.h"

class VAO
{
	unsigned int m_VAOID;

public:

	VAO();
	
	~VAO();

	void Bind() const;

	void Unbind() const;

	// TODO: currently our VAO does not support multiple VBOs
	void LinkVertexBuffer(const VBO& vbo, const VBOLayout& layout);
};

#endif // !VAO_H

