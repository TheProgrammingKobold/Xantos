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

    if (glm::length(movement) > 1.0f)
        movement = glm::normalize(movement);

    _speed = Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT) ? 20.0f : 5.0f;

    glm::vec3 horizontalVelocity(_velocity.x, 0.0f, _velocity.z);
    const glm::vec3 targetVelocity = movement * _speed;
    const bool hasMovementInput = glm::length(movement) > 0.0f;
    const float acceleration = hasMovementInput
        ? (_grounded ? _groundAcceleration : _airAcceleration)
        : (_grounded ? _groundFriction : _airFriction);

    const glm::vec3 velocityChange = targetVelocity - horizontalVelocity;
    const float velocityChangeLength = glm::length(velocityChange);
    const float maxVelocityChange = acceleration * deltaTime;
    if (velocityChangeLength > maxVelocityChange && velocityChangeLength > 0.0f)
        horizontalVelocity += velocityChange / velocityChangeLength * maxVelocityChange;
    else
        horizontalVelocity = targetVelocity;

    _velocity.x = horizontalVelocity.x;
    _velocity.z = horizontalVelocity.z;

    const bool jumpDown = Input::IsKeyDown(GLFW_KEY_SPACE);
    const bool jumpPressed = jumpDown && !_jumpWasDown;
    _jumpWasDown = jumpDown;

    if (_grounded)
    {
        _velocity.y = 0.0f;
        if (jumpPressed)
        {
            _velocity.y = _jumpSpeed;
            _grounded = false;
        }
    }

    if (!_grounded)
        _velocity.y -= _gravity * deltaTime;

    transform.position += _velocity * deltaTime;

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