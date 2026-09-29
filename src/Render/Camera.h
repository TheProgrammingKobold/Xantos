#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

enum class ProjectionType
{
    Perspective,
    Orthographic
};

class Camera
{
public:

    Camera(
        int width,
        int height,
        const glm::vec3& position
    );

    void UpdateMatrix();

    void Rotate(float yaw, float pitch);
    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;

    void SetPosition(const glm::vec3& position);
    void SetOrientation(const glm::vec3& orientation);

    void SetWidth(int width);
    void SetHeight(int height);
    void SetWidthHeight(int width, int height);

    const glm::vec3& GetPosition() const;
    const glm::vec3& GetOrientation() const;

    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetPerspectiveProjection() const;
    const glm::mat4& GetOrthoProjection() const;
    const glm::mat4& GetMatrix() const;

private:

    int _width;
    int _height;

    float _yaw = 0.0f;
    float _pitch = 0.0f;

    glm::vec3 _Position;
    glm::vec3 _Orientation = { 0.0f, 0.0f, -1.0f };
    glm::vec3 _Up = { 0.0f, 1.0f, 0.0f };

    glm::mat4 _viewMatrix = glm::mat4(1.0f);
    glm::mat4 _perspectiveProjection = glm::mat4(1.0f);
    glm::mat4 _orthoProjection = glm::mat4(1.0f);
    glm::mat4 _cameraMatrix = glm::mat4(1.0f);

    float _FOVdegree = 90.0f;
    float _nearPlane = 0.1f;
    float _farPlane = 1000.0f;
};