#pragma once

#include "../Core/RenderAssetTypes.h"

class RenderAssetManager;
class Shader;

class Material
{
public:

    Material(
        ShaderID shader,
        TextureID texture
    );

    void Bind(
        RenderAssetManager& assets
    );

    Shader& GetShader(
        RenderAssetManager& assets
    );

    ShaderID GetShaderID() const noexcept
    {
        return _shader;
    }

    TextureID GetTextureID() const noexcept
    {
        return _texture;
    }

private:

    ShaderID _shader;
    TextureID _texture;
};