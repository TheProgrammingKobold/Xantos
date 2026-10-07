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
        ? 20.0f * 8
        : 5.0f * 8;


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
    const float dt = fixedDeltaTime;


    // --------------------------------
    // Current terrain
    // --------------------------------

    const float groundY =
        terrainGenerator.GetInterpolatedHeight(
            transform.position.x,
            transform.position.z
        );

    const glm::vec3 groundNormal =
        terrainGenerator.GetInterpolatedNormal(
            transform.position.x,
            transform.position.z
        );

    const float playerBottom =
        transform.position.y + _aabb.min.y;


    // --------------------------------
    // Check whether we are still touching
    // the ground BEFORE applying movement
    // --------------------------------

    if (_grounded)
    {
        if (playerBottom >
            groundY + 0.05f)
        {
            _grounded = false;
        }
    }


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
    // Movement
    // --------------------------------

    const bool hasMovementInput =
        glm::length(_movementInput) > 0.0f;

    const glm::vec3 desiredHorizontalVelocity =
        _movementInput * _speed * 10.0f;


    if (_grounded)
    {
        // --------------------------------
        // Stay constrained to the terrain
        // --------------------------------

        //
        // Remove the component of velocity that
        // points through the terrain.
        //
        _velocity -=
            groundNormal *
            glm::dot(_velocity, groundNormal);


        // --------------------------------
        // Gravity along the surface
        // --------------------------------

        const glm::vec3 gravity(
            0.0f,
            -_gravity,
            0.0f
        );

        const glm::vec3 gravityAlongSurface =
            gravity -
            groundNormal *
            glm::dot(gravity, groundNormal);

        _velocity +=
            gravityAlongSurface * dt;


        // --------------------------------
        // Convert player input into a
        // velocity ALONG the ramp
        // --------------------------------

        glm::vec3 targetVelocity =
            desiredHorizontalVelocity;

        targetVelocity -=
            groundNormal *
            glm::dot(targetVelocity, groundNormal);


        // --------------------------------
        // Accelerate toward target
        // --------------------------------

        const float acceleration =
            hasMovementInput
            ? _groundAcceleration
            : _groundFriction;

        const glm::vec3 velocityChange =
            targetVelocity - _velocity;

        const float changeLength =
            glm::length(velocityChange);

        const float maxChange =
            acceleration * dt;

        if (changeLength > maxChange &&
            changeLength > 0.0f)
        {
            _velocity +=
                velocityChange /
                changeLength *
                maxChange;
        }
        else
        {
            _velocity = targetVelocity;
        }
    }
    else
    {
        // --------------------------------
        // Air movement
        // --------------------------------

        glm::vec3 horizontalVelocity(
            _velocity.x,
            0.0f,
            _velocity.z
        );

        const glm::vec3 targetVelocity =
            desiredHorizontalVelocity;

        const glm::vec3 velocityChange =
            targetVelocity -
            horizontalVelocity;

        const float changeLength =
            glm::length(velocityChange);

        const float maxChange =
            (_movementInput.length() > 0.0f
                ? _airAcceleration
                : _airFriction) * dt;

        if (changeLength > maxChange &&
            changeLength > 0.0f)
        {
            horizontalVelocity +=
                velocityChange /
                changeLength *
                maxChange;
        }
        else
        {
            horizontalVelocity =
                targetVelocity;
        }

        _velocity.x = horizontalVelocity.x;
        _velocity.z = horizontalVelocity.z;


        // --------------------------------
        // Gravity
        // --------------------------------

        _velocity.y -=
            _gravity * dt;
    }


    // --------------------------------
    // Integrate
    // --------------------------------

    transform.position +=
        _velocity * dt;


    // --------------------------------
    // Terrain collision AFTER movement
    // --------------------------------

    const float newGroundY =
        terrainGenerator.GetInterpolatedHeight(
            transform.position.x,
            transform.position.z
        );

    const glm::vec3 newGroundNormal =
        terrainGenerator.GetInterpolatedNormal(
            transform.position.x,
            transform.position.z
        );

    const float newPlayerBottom =
        transform.position.y + _aabb.min.y;


    // --------------------------------
    // We hit the terrain
    // --------------------------------

    if (newPlayerBottom < newGroundY)
    {
        // Push the player out of the terrain.
        transform.position.y =
            newGroundY - _aabb.min.y;


        // Remove only velocity that is pointing
        // INTO the terrain.
        const float velocityIntoSurface =
            glm::dot(
                _velocity,
                newGroundNormal
            );

        if (velocityIntoSurface < 0.0f)
        {
            // Reflect the velocity away from the surface.
            _velocity -=
                newGroundNormal *
                ((1.0f + _bounciness) *
                    velocityIntoSurface);

            // We're bouncing, not grounded.
            _grounded = false;
        }
        else
        {
            _grounded = true;
        }
    }
    else
    {
        // The ground disappeared from underneath us.
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