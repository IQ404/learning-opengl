#ifndef VBO_H
#define VBO_H

class VBO
{
	unsigned int m_RendererID = 0;

public:

	VBO(const void* data, unsigned int size);	// size in bytes; bind after initialization
	
	~VBO();

	void Bind() const;
	
	void Unbind() const;
};

#endif // !VBO_H
