#include "Material.h"

Material::Material(
    std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture
)
    : _shader(std::move(shader)),
    _texture(std::move(texture))
{
}

void Material::Bind()
{
    _shader->activate();

    if (_texture)
    {
        _texture->bind();

        _texture->texUnit(
            *_shader,
            "tex",
            0
        );
    }
}

Shader& Material::GetShader()
{
    return *_shader;
}