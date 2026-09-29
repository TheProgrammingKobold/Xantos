#include "Texture.h"

#include <iostream>
#include <stb_image.h>
#include <filesystem>
#include "../Util/File.h"

/*
* ------------
* Texture
* ------------
*/


Texture::Texture(const std::string& fileName)
{
	type = GL_TEXTURE_2D;
	m_slot = GL_TEXTURE0;
	int width, height, numColCh;

	stbi_set_flip_vertically_on_load(true);

	std::string filePath = Util::RootFilePath() + "\\Assets\\Textures\\" + fileName;

	if (!std::filesystem::is_directory(filePath) && !std::filesystem::exists(filePath))
	{
		std::cerr << "ERROR: Couldn't load file at path '" << filePath << "'\n";
		// TODO: Change this default texture to a path thats relevent to the project
		filePath = Util::RootFilePath() + "extern/Textures/Default.png";
	}

	uint8_t* imageBytes = stbi_load(filePath.c_str(), &width, &height, &numColCh, 0);

	if (!imageBytes)
	{
		std::cerr << "ERROR: Failed to load image data!\n";
	}

	int format;

	switch (numColCh)
	{
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		std::cerr << "ERROR: Unsupported number of color channels!\n";
	}

	glGenTextures(1, &ID);

	Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, imageBytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageBytes);

	Unbind();
}

const void Texture::UpdateTexture(GLenum format, GLenum pixelType, int width, int height, const void* data) const
{
	Bind();
	glTexImage2D(type, 0, GL_RGBA, width, height, 0, format, pixelType, data);
	glGenerateMipmap(type);
	Unbind();
}

const void Texture::TexUnit(Shader& shader, const char* uniform, GLuint unit) const
{
	shader.Activate();
	glUniform1i(glGetUniformLocation(shader.GetID(), uniform), unit);
}

void Texture::SetActiveTexture() const
{
	glActiveTexture(m_slot);
}

const void Texture::Bind() const
{
	glBindTexture(type, ID);
}

const void Texture::Unbind() const
{
	glBindTexture(type, 0);
}