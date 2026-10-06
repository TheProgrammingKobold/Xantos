#include "RenderAssetManager.h"

MeshID RenderAssetManager::LoadMesh(
    const std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices
)
{
    const uint32_t id = _nextMeshID++;

    _meshes[id] =
        std::make_unique<Mesh>(
            vertices,
            indices
        );

    return MeshID{ id };
}

TextureID RenderAssetManager::LoadTexture(
    const std::string& texture
)
{
    const uint32_t id = _nextTextureID++;

    _textures[id] =
        std::make_unique<Texture>(
            texture
        );

    return TextureID{ id };
}

ShaderID RenderAssetManager::LoadShader(
    const std::string& vertexShader,
    const std::string& fragmentShader
)
{
    const uint32_t id = _nextShaderID++;

    _shaders[id] =
        std::make_unique<Shader>(
            vertexShader,
            fragmentShader
        );

    return ShaderID{ id };
}

MaterialID RenderAssetManager::LoadMaterial(
    ShaderID shader,
    TextureID texture
)
{
    const uint32_t id = _nextMaterialID++;

    _materials[id] =
        std::make_unique<Material>(
            shader,
            texture
        );

    return MaterialID{ id };
}

void RenderAssetManager::DestroyMesh(
    MeshID id
)
{
    if (!id)
        return;

    auto it =
        _meshes.find(id.value);

    if (it == _meshes.end())
    {
        throw std::runtime_error(
            "Tried to destroy nonexistent MeshID: " +
            std::to_string(id.value)
        );
    }

    _meshes.erase(it);
}

Mesh& RenderAssetManager::GetMesh(
    MeshID id
)
{
    auto it =
        _meshes.find(id.value);

    if (it == _meshes.end())
    {
        throw std::runtime_error(
            "Invalid MeshID: " +
            std::to_string(id.value)
        );
    }

    return *it->second;
}

Texture& RenderAssetManager::GetTexture(
    TextureID id
)
{
    return *_textures.at(id.value);
}

Shader& RenderAssetManager::GetShader(
    ShaderID id
)
{
    return *_shaders.at(id.value);
}

Material& RenderAssetManager::GetMaterial(
    MaterialID id
)
{
    return *_materials.at(id.value);
}