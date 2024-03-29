#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

class IndexBuffer
{
	unsigned int m_IBOID = 0;
	unsigned int m_IndicesCount = 0;

public:

	/*
	Currently we assume all index buffers contains data of type unsigned int
	*/
	IndexBuffer(const unsigned int* data, unsigned int count);	// count is the number of indices; bind after initialization
	
	~IndexBuffer();

	void Bind() const;
	
	void Unbind() const;

	unsigned int GetIndicesCount() const;
};

#endif // !INDEXBUFFER_H
