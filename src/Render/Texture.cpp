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

Texture::Texture(GLenum texType, GLenum slot, GLenum format, GLenum pixelType, std::string fileName)
	:type(texType), m_slot(slot)
{
	int width, height, numColCh;

	stbi_set_flip_vertically_on_load(true);

	std::string filePath = Util::RootFilePath() + "\\Assets\\Textures\\" + fileName;

	if (!std::filesystem::is_directory(filePath) && !std::filesystem::exists(filePath))
	{
		std::cerr << "ERROR: Couldn't load file at path '" << filePath << "'\n";
		filePath = Util::RootFilePath() + "extern/Textures/Default.png";
	}

	uint8_t* imageBytes = stbi_load(filePath.c_str(), &width, &height, &numColCh, 0);

	if (!imageBytes)
	{
		std::cerr << "ERROR: Failed to load image data!\n";
	}

	glGenTextures(1, &ID);

	Bind();

	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(type, 0, GL_RGBA, width, height, 0, format, pixelType, imageBytes);
	glGenerateMipmap(type);

	stbi_image_free(imageBytes);

	Unbind();
}

// Constructor delegation ensures the base constructor initializes members, avoiding temporary objects.
Texture::Texture(TextureInfo info)
	:Texture(info.type, info.slot, info.format, info.pixelType, info.fileName)
{
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