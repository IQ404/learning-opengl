#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{
	unsigned int m_TextureObjectID = 0;
	std::string m_FilePath;
	unsigned char* m_CPUBuffer = nullptr;
	int m_Width = 0;
	int m_Height = 0;
	int m_BytesPerPixel = 0;

public:

	Texture(const std::string& file_path);
	
	~Texture();

	void Bind(unsigned int slot = 0) const;

	void Unbind() const;

	int GetWidth() const
	{
		return m_Width;
	}

	int GetHeight() const
	{
		return m_Height;
	}
};


#endif // !TEXTURE_H
