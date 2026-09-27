#pragma once

#include "Mesh.h"
#include "Material.h"

#include <glm/glm.hpp>
#include <memory>

struct DrawCommand
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    glm::mat4 transform;
};

struct TextCommand
{
    std::string text;
    glm::vec2 position;
    float scale;
    glm::vec3 color;
};