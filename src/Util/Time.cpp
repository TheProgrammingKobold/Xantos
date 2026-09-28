#include "Time.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float Util::GetDeltaTime()
{
	return deltaTime;
}

void Util::UpdateDeltaTime()
{
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}
