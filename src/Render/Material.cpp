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
    _shader->Activate();

    if (_texture)
    {
        _texture->Bind();

        _texture->TexUnit(
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