#pragma once

#include "VBO.h"

class VAO
{
public:

	VAO();

	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void Bind() const;
	void Unbind() const;
	void DeleteObject() const;

private:

	GLuint _ID;

};