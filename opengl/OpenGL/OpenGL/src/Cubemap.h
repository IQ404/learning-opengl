#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>

class Cubemap
{
	unsigned int m_CubemapID = 0;
	std::string m_SixFacesTexturesFilePaths[6];
	unsigned char* m_CPUBuffer = nullptr;	// right now we aren't keeping the textures data on CPU
	int m_Width = 0;	// right now this is just a intermediate data holder
	int m_Height = 0;	// right now this is just a intermediate data holder
	int m_BytesPerPixel = 0;

public:

	Cubemap(
		const std::string& file_path_PosX,
		const std::string& file_path_NegX,
		const std::string& file_path_PosY,
		const std::string& file_path_NegY,
		const std::string& file_path_PosZ,
		const std::string& file_path_NegZ
	);
	
	~Cubemap();

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


#endif // !CUBEMAP_H
