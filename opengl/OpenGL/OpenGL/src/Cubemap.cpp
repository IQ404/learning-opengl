#include "Cubemap.h"
#include "DebugTools.h"
#include "stb_image/stb_image.h"

#include <iostream>

Cubemap::Cubemap(
	const std::string& file_path_PosX,
	const std::string& file_path_NegX,
	const std::string& file_path_PosY,
	const std::string& file_path_NegY,
	const std::string& file_path_PosZ,
	const std::string& file_path_NegZ
)
{
	m_SixFacesTexturesFilePaths[0] = file_path_PosX;
	m_SixFacesTexturesFilePaths[1] = file_path_NegX;
	m_SixFacesTexturesFilePaths[2] = file_path_PosY;
	m_SixFacesTexturesFilePaths[3] = file_path_NegY;
	m_SixFacesTexturesFilePaths[4] = file_path_PosZ;
	m_SixFacesTexturesFilePaths[5] = file_path_NegZ;

	GLCall(glGenTextures(1, &m_CubemapID));
	// Here we associate the texture to the target GL_TEXTURE_CUBE_MAP of the default texture unit (slot): 0
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID));

	// Set up the necessary settings for the bound texture (those are states of the texture object, not the texture unit):
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));	// S is the X (horizontal) for texture
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));	// T is the Y (vertical) for texture
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));	// R is the Z for texture

	// TODO: Why do we need to set for GL_TEXTURE_WRAP_R?
	// TODO: For cubemap, when would we sample outside the texture range? Ouside which face's range?

	stbi_set_flip_vertically_on_load(0);	// TODO: why don't we need filpping?

	// Send the texture data from CPU to GPU:
	for (unsigned int i = 0; i < 6; i++)
	{
		m_CPUBuffer = stbi_load(m_SixFacesTexturesFilePaths[i].c_str(), &m_Width, &m_Height, &m_BytesPerPixel, 4);	// 4 for rgba, if the file has only rgb available, a will be set to 255.
		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_CPUBuffer));	// TODO: understand how texture coordinates map to cubemap faces
		stbi_image_free(m_CPUBuffer);
		m_CPUBuffer = nullptr;	// m_CPUBuffer != nullptr after the above line.
	}

	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));	// After this line, target GL_TEXTURE_CUBE_MAP on texture unit 0 is associated with the default texture object with ID 0.
}

Cubemap::~Cubemap()
{
	GLCall(glDeleteTextures(1, &m_CubemapID));	// relese the memory storing the texture data on the GPU
}

void Cubemap::Bind(unsigned int slot /* = 0 */) const
// Associate the GL_TEXTURE_CUBE_MAP target of the provided texture unit (slot) to the texture object represented by this Texture
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID));
}

void Cubemap::Unbind() const
// Associate the GL_TEXTURE_CUBE_MAP target of the active texture unit to the default texture object with ID 0
{
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}
