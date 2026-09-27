#pragma once

#include "VBO.h"

class VAO
{
public:

	VAO();

	void linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void bind() const;
	void unbind() const;
	void deleteObject() const;

private:

	GLuint _ID;

};