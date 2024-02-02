#include "Texture.h"
#include "DebugTools.h"
#include "stb_image/stb_image.h"

#include <iostream>

Texture::Texture(const std::string& file_path)
	: m_FilePath{ file_path }
{
	stbi_set_flip_vertically_on_load(1);	// this is due to that we are loading PNG file (origin at top left) and OpenGL texture's origin is at bottom left

	m_CPUBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BytesPerPixel, 4);	// 4 for rgba

	GLCall(glGenTextures(1, &m_TextureObjectID));
	// Here we associate the texture to the target GL_TEXTURE_2D of the default texture unit (slot): 0
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureObjectID));
	// Set up the necessary settings for the bound texture (those are states of the texture object, not the texture unit):
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));	// S is the X (horizontal) for texture
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));	// T is the Y (vertical) for texture
	// Send the texture data from CPU to GPU:
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_CPUBuffer));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));	// After this line, target GL_TEXTURE_2D on texture unit 0 is associated with the default texture object with ID 0.

	// If we don't want to retain a copy of the pixel data of the texture:
	if (m_CPUBuffer)
	{
		stbi_image_free(m_CPUBuffer);
		m_CPUBuffer = nullptr;	// m_CPUBuffer != nullptr after the above line.
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_TextureObjectID));	// relese the memory storing the texture data on the GPU
}

void Texture::Bind(unsigned int slot /* = 0 */) const
// Associate the GL_TEXTURE_2D target of the provided texture unit (slot) to the texture object represented by this Texture
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureObjectID));
}

void Texture::Unbind() const
// Associate the GL_TEXTURE_2D target of the active texture unit to the default texture object with ID 0
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
