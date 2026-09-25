#pragma once

#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>

class Shader
{
public:

	Shader(std::string filePathToVertex, std::string filePathToFragment);

	void activate();
	void deleteShader();

	const void setMatrix(const glm::mat4& matrix, const std::string& uniform) const;
	const void setVec3(const glm::vec3& vec, const std::string& uniform) const;
	const void setInt(int value, const std::string& uniform) const;
	const void setFloat(float value, const std::string& uniform) const;
	const void setFloatArray(const std::vector<GLfloat>& data, const std::string& uniform) const;

	inline const GLuint getID() const { return _ID; }
	inline const GLint getUniform(std::string input) const { return glGetUniformLocation(_ID, input.c_str()); }

private:

	static std::string _parseFileToString(std::string filepath);
	static void _validateShader(GLuint shader);

private:

	GLuint _ID = 0;

};
