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

	void Activate();
	void DeleteShader();

	const void SetMatrix(const glm::mat4& matrix, const std::string& uniform) const;
	const void SetVec3(const glm::vec3& vec, const std::string& uniform) const;
	const void SetInt(int value, const std::string& uniform) const;
	const void SetFloat(float value, const std::string& uniform) const;
	const void SetFloatArray(const std::vector<GLfloat>& data, const std::string& uniform) const;

	inline const GLuint GetID() const { return _ID; }
	inline const GLint GetUniform(std::string input) const { return glGetUniformLocation(_ID, input.c_str()); }

private:

	static std::string ParseFileToString(std::string filepath);
	static void ValidateShader(GLuint shader);

private:

	GLuint _ID = 0;

};
