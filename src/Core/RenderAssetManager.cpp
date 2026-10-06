#include "RenderAssetManager.h"


MeshID RenderAssetManager::LoadMesh(
    const std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices
)
{
    uint32_t id = _nextMeshID++;

    _meshes[id] = std::make_unique<Mesh>(
        vertices,
        indices
    );

    return MeshID{ id };
}

TextureID RenderAssetManager::LoadTexture(
    const std::string& texture
)
{
    uint32_t id = _nextTextureID++;

    _textures[id] = std::make_unique<Texture>(
        texture
    );

    return TextureID{ id };
}


ShaderID RenderAssetManager::LoadShader(
    const std::string& vertexShader,
    const std::string& fragmentShader
)
{
    uint32_t id = _nextShaderID++;

    _shaders[id] = std::make_unique<Shader>(
        vertexShader,
        fragmentShader
    );

    return ShaderID{ id };
}


Mesh& RenderAssetManager::GetMesh(MeshID id)
{
    return *_meshes.at(id.value);
}


Texture& RenderAssetManager::GetTexture(TextureID id)
{
    return *_textures.at(id.value);
}


Shader& RenderAssetManager::GetShader(ShaderID id)
{
    return *_shaders.at(id.value);
}