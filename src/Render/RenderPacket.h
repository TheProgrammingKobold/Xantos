#pragma once

#include "DrawCommand.h"

#include <glm/glm.hpp>

#include <vector>

struct RenderCameraState
{
    glm::vec3 position{ 0.0f };

    float yaw = 0.0f;
    float pitch = 0.0f;

    int width = 1;
    int height = 1;
};

struct RenderPacket
{
    uint64_t frameNumber = 0;

    RenderCameraState camera;

    std::vector<DrawCommand>
        drawCommands;

    std::vector<TextCommand>
        textCommands;
};