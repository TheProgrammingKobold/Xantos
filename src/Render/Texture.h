#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

struct TextureInfo
{
	TextureInfo() = default;
	TextureInfo(GLenum type, GLenum slot, GLenum format, GLenum pixelType, std::string fileName)
	{
		this->type = type;
		this->slot = slot;
		this->format = format;
		this->pixelType = pixelType;
		this->fileName = fileName;
	}

	GLenum type;
	GLenum slot;
	GLenum format;
	GLenum pixelType;
	std::string fileName;
};

class Texture
{
public:

	Texture() = default;
	Texture(GLenum texType, GLenum slot, GLenum format, GLenum pixelType, std::string fileName);
	Texture(TextureInfo info);

	const void updateTexture(GLenum format, GLenum pixelType, int width, int height, const void* data) const;

	const void texUnit(Shader& shader, const char* uniform, GLuint unit) const;

	void setActiveTexture() const;

	const void bind() const;

	const void unbind() const;

	inline const GLuint getID() const { return ID; };

private:

	GLuint ID;
	GLenum type;
	GLenum m_slot;

};