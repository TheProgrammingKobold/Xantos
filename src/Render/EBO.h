#pragma once

#include <vector>
#include <glad/glad.h>

class EBO
{
public:

	EBO(const std::vector<GLuint>& indices);

	void Bind();
	void Unbind();
	void DeleteObject();

private:
	GLuint _ID;
};
