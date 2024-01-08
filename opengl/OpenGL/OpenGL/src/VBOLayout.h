#ifndef VBOLAYOUT_H
#define VBOLAYOUT_H

#include <iostream>
#include <vector>
#include "DebugTools.h"

struct VBOAttribute
{
	unsigned int opengl_type;
	unsigned int components_count;
	unsigned char normalized;

	static unsigned int SizeOfOpenGLType(unsigned int opengl_type)
	{
		switch (opengl_type)
		{
		case GL_FLOAT:
			return 4;
		case GL_UNSIGNED_INT:
			return 4;
		case GL_UNSIGNED_BYTE:
			return 1;
		default:
			std::cout << "Error: SizeOfType(unsigned int type) - the provided type is currently not supported." << std::endl;
			ASSERT_DebugBreak_MSVC(false);
			return 0;
		}
	}
};

class VBOLayout
{
	unsigned int m_Stride = 0;
	std::vector<VBOAttribute> m_Attributes;

public:

	VBOLayout()
	{

	}

	const std::vector<VBOAttribute> GetAttributes() const
	{
		return m_Attributes;
	}

	unsigned int GetStride() const
	{
		return m_Stride;
	}

	template<typename T>
	void AddAttribute(unsigned int components_count)
	{
		//static_assert(false, "Adding data type that is not implemented for VBOLayout");
		// TODO: how to use static_assert here without always triggering compiler error?
		
		std::cout << "Error: Adding data type that is not implemented for VBOLayout" << std::endl;
		ASSERT_DebugBreak_MSVC(false);
	}

	template<>
	void AddAttribute<float>(unsigned int components_count)
	{
		m_Attributes.push_back({ GL_FLOAT, components_count, GL_FALSE });	// TODO: right now normalized is hard-coded to false
		m_Stride += VBOAttribute::SizeOfOpenGLType(GL_FLOAT) * components_count;
	}

	template<>
	void AddAttribute<unsigned int>(unsigned int components_count)
	{
		m_Attributes.push_back({ GL_UNSIGNED_INT, components_count, GL_FALSE });	// TODO: right now normalized is hard-coded to false
		m_Stride += VBOAttribute::SizeOfOpenGLType(GL_UNSIGNED_INT) * components_count;
	}

	template<>
	void AddAttribute<unsigned char>(unsigned int components_count)
	{
		m_Attributes.push_back({ GL_UNSIGNED_BYTE, components_count, GL_TRUE });	// TODO: right now normalized is hard-coded to true
		m_Stride += VBOAttribute::SizeOfOpenGLType(GL_UNSIGNED_BYTE) * components_count;
	}

};

#endif // !VBOLAYOUT_H

