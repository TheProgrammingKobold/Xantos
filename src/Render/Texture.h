#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class Texture
{
public:

	Texture() = default;
	explicit Texture(const std::string& fileName);

	const void UpdateTexture(GLenum format, GLenum pixelType, int width, int height, const void* data) const;

	const void TexUnit(Shader& shader, const char* uniform, GLuint unit) const;

	void SetActiveTexture() const;

	const void Bind() const;

	const void Unbind() const;

	inline const GLuint GetID() const { return ID; };

private:

	GLuint ID;
	GLenum type;
	GLenum m_slot;

};