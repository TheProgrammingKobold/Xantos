#include "Player.h"

#include "../../Core/Input.h"

Player::Player()
{
}

void Player::Update(float deltaTime)
{
    glm::vec3 forward = _orientation;
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    glm::vec3 right = glm::normalize(
        glm::cross(
            forward,
            glm::vec3(0.0f, 1.0f, 0.0f)
        )
    );

    glm::vec3 movement(0.0f);

    if (Input::IsKeyDown(GLFW_KEY_W))
        movement += forward;

    if (Input::IsKeyDown(GLFW_KEY_S))
        movement -= forward;

    if (Input::IsKeyDown(GLFW_KEY_D))
        movement += right;

    if (Input::IsKeyDown(GLFW_KEY_A))
        movement -= right;

    if (Input::IsKeyDown(GLFW_KEY_SPACE))
        movement += glm::vec3(0.0f, 1.0f, 0.0f);

    if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
    {
        movement -= glm::vec3(0.0f, 1.0f, 0.0f);
    }

	if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		_speed = 20.0f;
	}
	else
	{
		_speed = 5.0f;
	}

    if (glm::length(movement) > 0.0f)
        movement = glm::normalize(movement);

    transform.position += movement * _speed * deltaTime;

    if (Input::IsMouseCaptured())
    {
        glm::vec2 mouseDelta = Input::GetMouseDelta();

        constexpr float sensitivity = 0.002f;

        _yaw += mouseDelta.x * sensitivity;
        _pitch -= mouseDelta.y * sensitivity;

        _pitch = glm::clamp(
            _pitch,
            glm::radians(-89.0f),
            glm::radians(89.0f)
        );
    }

}

void Player::SetOrientation(const glm::vec3& orientation)
{
    _orientation = orientation;
    _orientation.y = 0.0f;

    if (glm::length(_orientation) > 0.0f)
        _orientation = glm::normalize(_orientation);
}