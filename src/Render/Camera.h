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

    void updateMatrix();

    void setPosition(const glm::vec3& position);
    void setOrientation(const glm::vec3& orientation);

    void setWidth(int width);
    void setHeight(int height);
    void setWidthHeight(int width, int height);

    const glm::vec3& getPosition() const;
    const glm::vec3& getOrientation() const;

    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getPerspectiveProjection() const;
    const glm::mat4& getOrthoProjection() const;
    const glm::mat4& getMatrix() const;

private:

    int _width;
    int _height;

    glm::vec3 _Position;
    glm::vec3 _Orientation = { 0.0f, 0.0f, -1.0f };
    glm::vec3 _Up = { 0.0f, 1.0f, 0.0f };

    glm::mat4 _viewMatrix = glm::mat4(1.0f);
    glm::mat4 _perspectiveProjection = glm::mat4(1.0f);
    glm::mat4 _orthoProjection = glm::mat4(1.0f);
    glm::mat4 _cameraMatrix = glm::mat4(1.0f);

    float _FOVdegree = 90.0f;
    float _nearPlane = 0.1f;
    float _farPlane = 100.0f;
};