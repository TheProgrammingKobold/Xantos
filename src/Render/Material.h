#pragma once

#include "Shader.h"
#include "Texture.h"

#include <memory>

class Material
{
public:
    Material(
        std::shared_ptr<Shader> shader,
        std::shared_ptr<Texture> texture = nullptr
    );

    void Bind();

    Shader& GetShader();

private:
    std::shared_ptr<Shader> _shader;
    std::shared_ptr<Texture> _texture;
};