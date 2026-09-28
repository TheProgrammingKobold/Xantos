#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(std::string filePathToVertex, std::string filePathToFragment)
{
	// Reading the shaders from the file
	std::string vertexString = ParseFileToString("Shaders/" + filePathToVertex);
	std::string fragmentString = ParseFileToString("Shaders/" + filePathToFragment);

	if (vertexString == "ERROR" || fragmentString == "ERROR")
	{
		std::cerr << "[ERROR]: VERTEXSTRING: " << vertexString << '\n' << "FRAGMENTSTRING: " << fragmentString << '\n';
		return;
	}

	// Converts them into const char pointers
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	// Creates and ID for the shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attaches the vertex shader source to a vertex object
	glShaderSource(vertexShader, 1, &vertexCode, NULL);
	// Compiles the shader into machine code
	glCompileShader(vertexShader);
	// Validates the shader
	ValidateShader(vertexShader);


	// Creates ID for fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attaches the fragment shader to a fragment object
	glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
	// Compiling the shader into machine code
	glCompileShader(fragmentShader);
	// Validates the shader
	ValidateShader(fragmentShader);


	// Creates a shader program and the ID to it
	_ID = glCreateProgram();
	// Attaches both shaders to the shader program
	// MISTAKE! Was accidently attatching the fragment shader twice :c
	glAttachShader(_ID, vertexShader);
	glAttachShader(_ID, fragmentShader);
	// Links everything
	glLinkProgram(_ID);
	// Checks if the shaders linked properly

	GLint hasCompiled;
	char infoLog[1024];
	glGetProgramiv(_ID, GL_LINK_STATUS, &hasCompiled);
	if (!hasCompiled)
	{
		glGetProgramInfoLog(_ID, 1024, NULL, infoLog);
		std::cerr << "[Shader Linking Error]: " << infoLog << '\n';
	}


	// Deletes the shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Shader::Activate()
{
	glUseProgram(_ID);
}

void Shader::DeleteShader()
{
	glDeleteProgram(_ID);
}

const void Shader::SetMatrix(const glm::mat4& matrix, const std::string& uniform) const
{
	glUniformMatrix4fv(GetUniform(uniform), 1, GL_FALSE, glm::value_ptr(matrix));
}

const void Shader::SetVec3(const glm::vec3& vec, const std::string& uniform) const
{
	glUniform3fv(GetUniform(uniform), 1, glm::value_ptr(vec));
}

const void Shader::SetInt(int value, const std::string& uniform) const
{
	glUniform1i(GetUniform(uniform), value);
}

const void Shader::SetFloat(float value, const std::string& uniform) const
{
	glUniform1f(GetUniform(uniform), value);
}

const void Shader::SetFloatArray(const std::vector<GLfloat>& data, const std::string& uniform) const
{
	glUniform1fv(GetUniform(uniform), data.size(), data.data());
}

std::string Shader::ParseFileToString(std::string filepath)
{
	// Setting the result
	std::string result;

	// Setting the main file path object from filepath parameter
	std::filesystem::path filePath = filepath;

	// Checking if the file actually exists or not
	if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
	{
		// Opens the file for reading
		std::ifstream inputFile(filePath);

		// Checks if the file opened properly
		if (inputFile.is_open())
		{
			// Reads the entire content of the file and then puts that into a string
			std::ostringstream contentStream;
			contentStream << inputFile.rdbuf();
			result = contentStream.str();

			inputFile.close();
		}
		else {
			std::cerr << "Unable to open file \"" + filepath + "\"" << '\n';
			result = "ERROR";
		}
	}
	else {
		std::cerr << "File at \"" + filepath + "\" was unable to be reached" << '\n';
		result = "ERROR";
	}

	return result;
}

void Shader::ValidateShader(GLuint shader)
{
	int status;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) // Status works like a bool here
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "[Shader Compilation Error]: " << infoLog << '\n';
	}
}