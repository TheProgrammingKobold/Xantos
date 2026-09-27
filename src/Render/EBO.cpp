#include "EBO.h"

EBO::EBO(const std::vector<GLuint>& indices)
{
	glGenBuffers(1, &_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void EBO::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ID);
}

void EBO::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::deleteObject()
{
	glDeleteBuffers(1, &_ID);
}