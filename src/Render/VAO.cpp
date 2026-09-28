#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &_ID);
}

// Links the VBO to the VAO using a certain layout. Layout also being used in shader under "location"
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, (GLsizei)stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind() const
{
	glBindVertexArray(_ID);
}

void VAO::Unbind() const
{
	glBindVertexArray(0);
}

void VAO::DeleteObject() const
{
	glDeleteVertexArrays(1, &_ID);
}