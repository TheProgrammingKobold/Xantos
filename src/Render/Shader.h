#pragma once

#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:

	Shader(std::string filePathToVertex, std::string filePathToFragment);

	void activate();
	void deleteShader();

	inline const GLuint getID() const { return _ID; }
	inline const GLint getUniform(std::string input) const { return glGetUniformLocation(_ID, input.c_str()); }

private:

	static std::string _parseFileToString(std::string filepath);
	static void _validateShader(GLuint shader);

private:

	GLuint _ID = 0;

};
