#pragma once

#include "Cubemap.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

#include <glm/glm.hpp>

class Camera;

class SkyboxRenderer
{
public:
    SkyboxRenderer();

    void Render(
        const Camera& camera,
        const Cubemap& cubemap
    );

private:
    VAO _vao;
    VBO _vbo;
    Shader _shader;
};