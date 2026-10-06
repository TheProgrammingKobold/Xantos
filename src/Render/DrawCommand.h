#pragma once

#include "../Core/RenderAssetTypes.h"

#include <glm/glm.hpp>

#include <string>

struct DrawCommand
{
    MeshID mesh;
    MaterialID material;

    glm::mat4 transform{
        1.0f
    };
};

struct TextCommand
{
    std::string text;

    glm::vec2 position{
        0.0f
    };

    float scale = 1.0f;

    glm::vec3 color{
        1.0f
    };
};