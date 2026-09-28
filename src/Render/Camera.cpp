#include "Camera.h"

Camera::Camera(
    int width,
    int height,
    const glm::vec3& position
)
    : _width(width),
    _height(height),
    _Position(position)
{
    updateMatrix();
}

void Camera::updateMatrix()
{
    _viewMatrix = glm::lookAt(
        _Position,
        _Position + _Orientation,
        _Up
    );

    _perspectiveProjection = glm::perspective(
        glm::radians(_FOVdegree),
        static_cast<float>(_width) / static_cast<float>(_height),
        _nearPlane,
        _farPlane
    );

    _orthoProjection = glm::ortho(
        0.0f,
        static_cast<float>(_width),
        0.0f,
        static_cast<float>(_height)
    );

    _cameraMatrix = _perspectiveProjection * _viewMatrix;
}

void Camera::Rotate(float yaw, float pitch)
{
    _yaw = yaw;
    _pitch = pitch;

    _pitch = glm::clamp(
        _pitch,
        glm::radians(-89.0f),
        glm::radians(89.0f)
    );

    _Orientation = GetForward();
    updateMatrix();
}

glm::vec3 Camera::GetForward() const
{
    glm::vec3 forward;

    forward.x =
        cos(_pitch) * sin(_yaw);

    forward.y =
        sin(_pitch);

    forward.z =
        -cos(_pitch) * cos(_yaw);

    return glm::normalize(forward);
}

glm::vec3 Camera::GetRight() const
{
    return glm::normalize(
        glm::cross(
            GetForward(),
            glm::vec3(0.0f, 1.0f, 0.0f)
        )
    );
}

// --------------------
// Position / Direction
// --------------------

void Camera::setPosition(const glm::vec3& position)
{
    _Position = position;
    updateMatrix();
}

void Camera::setOrientation(const glm::vec3& orientation)
{
    _Orientation = glm::normalize(orientation);
    updateMatrix();
}

// --------------------
// Window dimensions
// --------------------

void Camera::setWidth(int width)
{
    _width = width;
    updateMatrix();
}

void Camera::setHeight(int height)
{
    _height = height;
    updateMatrix();
}

void Camera::setWidthHeight(int width, int height)
{
    _width = width;
    _height = height;
    updateMatrix();
}


// --------------------
// Getters
// --------------------

const glm::mat4& Camera::getPerspectiveProjection() const
{
    return _perspectiveProjection;
}

const glm::mat4& Camera::getOrthoProjection() const
{
    return _orthoProjection;
}

const glm::mat4& Camera::getViewMatrix() const
{
    return _viewMatrix;
}

const glm::mat4& Camera::getMatrix() const
{
    return _cameraMatrix;
}
