#pragma once

#include "../Entity/Entity.h"
#include "../../Core/AABB.h"

class Player : public Entity
{
public:
    Player();

    void Update(float deltaTime);

    void SetGrounded(bool isGrounded)
    {
        _grounded = isGrounded;
        if (_grounded && _velocity.y < 0.0f)
            _velocity.y = 0.0f;
    }

    float GetYaw() const { return _yaw; }
    float GetPitch() const { return _pitch; }

    void SetOrientation(const glm::vec3& orientation);

private:
    float _speed = 5.0f;

	glm::vec3 _velocity{ 0.0f, 0.0f, 0.0f };
    float _groundAcceleration = 30.0f;
    float _airAcceleration = 10.0f;
    float _groundFriction = 200.0f;
    float _airFriction = 20.0f;
    float _jumpSpeed = 6.5f;
    bool _jumpWasDown = false;

    float _yaw = 0.0f;
    float _pitch = 0.0f;
    glm::vec3 _orientation{ 0.0f, 0.0f, -1.0f };
    AABB _aabb = { {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f} };

    bool _grounded = false;
	const float _gravity = 9.81f;
};