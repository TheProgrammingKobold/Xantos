#pragma once

#include "../Core/RenderAssetManager.h"

#include <glm/glm.hpp>
#include <memory>

struct DrawCommand
{
    MeshID mesh;
    MaterialID material;

    glm::mat4 transform;
};

struct TextCommand
{
    std::string text;
    glm::vec2 position;
    float scale;
    glm::vec3 color;
};