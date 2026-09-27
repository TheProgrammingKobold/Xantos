#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &_ID);
}

// Links the VBO to the VAO using a certain layout. Layout also being used in shader under "location"
void VAO::linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, (GLsizei)stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.unbind();
}

void VAO::bind() const
{
	glBindVertexArray(_ID);
}

void VAO::unbind() const
{
	glBindVertexArray(0);
}

void VAO::deleteObject() const
{
	glDeleteVertexArrays(1, &_ID);
}