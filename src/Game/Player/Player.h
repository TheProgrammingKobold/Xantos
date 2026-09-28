#pragma once

#include "../Entity/Entity.h"

class Player : public Entity
{
public:
    Player();

    void Update(float deltaTime);

    float GetYaw() const { return _yaw; }
    float GetPitch() const { return _pitch; }

    void SetOrientation(const glm::vec3& orientation);

private:
    float _speed = 5.0f;

    float _yaw = 0.0f;
    float _pitch = 0.0f;
    glm::vec3 _orientation{ 0.0f, 0.0f, -1.0f };
};