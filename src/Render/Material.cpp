#include "Material.h"

#include "../Core/RenderAssetManager.h"

#include "Shader.h"
#include "Texture.h"

Material::Material(
    ShaderID shader,
    TextureID texture
)
    : _shader(shader),
    _texture(texture)
{
}

void Material::Bind(
    RenderAssetManager& assets
)
{
    Shader& shader =
        assets.GetShader(_shader);

    Texture& texture =
        assets.GetTexture(_texture);

    shader.Activate();

    texture.Bind();

    texture.TexUnit(
        shader,
        "tex",
        0
    );
}

Shader& Material::GetShader(
    RenderAssetManager& assets
)
{
    return assets.GetShader(_shader);
}