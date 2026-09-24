#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Shader.h"

class Camera
{
public:


	Camera(int width, int height, glm::vec3 position);


	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	void matrix(Shader& shader, const char* uniform);


	// Getters
	inline const glm::vec3 getPosition() const { return _Position; }

private:

	int _width, _height;

	// Stores the main vectors of the camera
	glm::vec3 _Position;
	glm::vec3 _Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 _Up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 _cameraMatrix = glm::mat4(1.0f);

};
