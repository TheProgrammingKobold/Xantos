#include "Player.h"

#include "../../Core/Input.h"

Player::Player()
{
}

void Player::Update(float)
{
    // --------------------------------
    // Movement input
    // --------------------------------

    glm::vec3 forward = _orientation;
    forward.y = 0.0f;

    if (glm::length(forward) > 0.0f)
        forward = glm::normalize(forward);

    glm::vec3 right = glm::normalize(
        glm::cross(
            forward,
            glm::vec3(0.0f, 1.0f, 0.0f)
        )
    );

    _movementInput = glm::vec3(0.0f);

    if (Input::IsKeyDown(GLFW_KEY_W))
        _movementInput += forward;

    if (Input::IsKeyDown(GLFW_KEY_S))
        _movementInput -= forward;

    if (Input::IsKeyDown(GLFW_KEY_D))
        _movementInput += right;

    if (Input::IsKeyDown(GLFW_KEY_A))
        _movementInput -= right;

    if (glm::length(_movementInput) > 1.0f)
        _movementInput = glm::normalize(_movementInput);


    // --------------------------------
    // Movement speed
    // --------------------------------

    _speed = Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT)
        ? 20.0f
        : 5.0f;


    // --------------------------------
    // Jump input
    // --------------------------------

    const bool jumpDown =
        Input::IsKeyDown(GLFW_KEY_SPACE);

    const bool jumpPressed =
        jumpDown && !_jumpWasDown;

    _jumpWasDown = jumpDown;

    if (jumpPressed)
        _jumpRequested = true;


    // --------------------------------
    // Mouse look
    // --------------------------------

    if (Input::IsMouseCaptured())
    {
        const glm::vec2 mouseDelta =
            Input::GetMouseDelta();

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

void Player::PhysicsUpdate(
    float fixedDeltaTime,
    const TerrainGenerator& terrainGenerator)
{
    // --------------------------------
    // Jump
    // --------------------------------

    if (_jumpRequested)
    {
        if (_grounded)
        {
            _velocity.y = _jumpSpeed;
            _grounded = false;
        }

        _jumpRequested = false;
    }


    // --------------------------------
    // Horizontal movement
    // --------------------------------

    const glm::vec3 targetVelocity =
        _movementInput * _speed * 100.0f;

    glm::vec3 horizontalVelocity(
        _velocity.x,
        0.0f,
        _velocity.z
    );

    const bool hasMovementInput =
        glm::length(_movementInput) > 0.0f;

    const float acceleration =
        hasMovementInput
        ? (_grounded
            ? _groundAcceleration
            : _airAcceleration)
        : (_grounded
            ? _groundFriction
            : _airFriction);

    const glm::vec3 velocityChange =
        targetVelocity - horizontalVelocity;

    const float velocityChangeLength =
        glm::length(velocityChange);

    const float maxVelocityChange =
        acceleration * fixedDeltaTime;

    if (velocityChangeLength > maxVelocityChange &&
        velocityChangeLength > 0.0f)
    {
        horizontalVelocity +=
            velocityChange / velocityChangeLength *
            maxVelocityChange;
    }
    else
    {
        horizontalVelocity = targetVelocity;
    }

    _velocity.x = horizontalVelocity.x;
    _velocity.z = horizontalVelocity.z;


    // --------------------------------
    // Gravity
    // --------------------------------

    if (!_grounded)
        _velocity.y -= _gravity * fixedDeltaTime;


    // --------------------------------
    // Move
    // --------------------------------

    transform.position +=
        _velocity * fixedDeltaTime;


    // --------------------------------
    // Terrain collision
    // --------------------------------

    const float groundY =
        terrainGenerator.GetInterpolatedHeight(
            transform.position.x,
            transform.position.z
        );

    const float playerBottom =
        transform.position.y + _aabb.min.y;

    if (playerBottom <= groundY)
    {
        transform.position.y =
            groundY - _aabb.min.y;

        _velocity.y = -_velocity.y * 0.75f;
        _grounded = true;
    }
    else
    {
        _grounded = false;
    }
}

void Player::SetOrientation(const glm::vec3& orientation)
{
    _orientation = orientation;
    _orientation.y = 0.0f;

    if (glm::length(_orientation) > 0.0f)
        _orientation = glm::normalize(_orientation);
}