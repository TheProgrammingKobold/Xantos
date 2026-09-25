#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
	:_width(width), _height(height), _Position(position)
{
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// inits matrices since otherwise they will null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(_Position, _Position + _Orientation, _Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)_width / _height, nearPlane, farPlane);

	// Sets new camera matrix
	_cameraMatrix = projection * view;
}

void Camera::updateMatrix()
{
	// inits matrices since otherwise they will null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(_Position, _Position + _Orientation, _Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(90.0f), (float)_width / _height, 0.1f, 100.0f);

	// Sets new camera matrix
	_cameraMatrix = projection * view;
}

void Camera::matrix(Shader& shader, const char* uniform)
{
	// Exports the camera matrix
	glUniformMatrix4fv(shader.getUniform(uniform), 1, GL_FALSE, glm::value_ptr(_cameraMatrix));
}
