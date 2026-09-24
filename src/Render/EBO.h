#pragma once

#include <vector>
#include <glad/glad.h>

class EBO
{
public:

	EBO(std::vector<GLuint>& indices);

	void bind();
	void unbind();
	void deleteObject();

private:
	GLuint _ID;
};
