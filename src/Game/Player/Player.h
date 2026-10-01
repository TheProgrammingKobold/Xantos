#pragma once

#include "../Entity/Entity.h"
#include "../../Core/AABB.h"
#include "../../Core/TerrainGenerator.h"

class Player : public Entity
{
public:
    Player();

    void Update(float deltaTime) override;
    void PhysicsUpdate(
        float fixedDeltaTime,
        const TerrainGenerator& terrainGenerator
    ) override;

    void SetGrounded(bool isGrounded)
    {
        _grounded = isGrounded;

        if (_grounded && _velocity.y < 0.0f)
            _velocity.y = 0.0f;
    }

    bool IsGrounded() const { return _grounded; }

    float GetYaw() const { return _yaw; }
    float GetPitch() const { return _pitch; }

    const AABB& GetAABB() const { return _aabb; }

    void SetOrientation(const glm::vec3& orientation);

private:
    // Movement intent
    glm::vec3 _movementInput{ 0.0f };
    bool _jumpRequested = false;

    // Movement
    float _speed = 5.0f;

    float _groundAcceleration = 30.0f;
    float _airAcceleration = 10.0f;

    float _groundFriction = 200.0f;
    float _airFriction = 20.0f;

    // Physics
    glm::vec3 _velocity{ 0.0f };

    float _jumpSpeed = 10.0f;
    float _gravity = 25.0f;

    bool _grounded = false;

    // Input state
    bool _jumpWasDown = false;

    // Camera
    float _yaw = 0.0f;
    float _pitch = 0.0f;

    glm::vec3 _orientation{ 0.0f, 0.0f, -1.0f };

    AABB _aabb = {
        { -0.5f, -2.0f, -0.5f },
        {  0.5f,  1.0f,  0.5f }
    };
};