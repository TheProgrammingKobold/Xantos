#pragma once

#include "RenderAssetTypes.h"

#include "../Render/Vertex.h"
#include "../Render/Mesh.h"
#include "../Render/Material.h"
#include "../Render/Texture.h"
#include "../Render/Shader.h"

#include <glad/glad.h>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class RenderAssetManager
{
public:

    MeshID LoadMesh(
        const std::vector<Vertex>& vertices,
        const std::vector<GLuint>& indices
    );

    TextureID LoadTexture(
        const std::string& texture
    );

    ShaderID LoadShader(
        const std::string& vertexShader,
        const std::string& fragmentShader
    );

    MaterialID LoadMaterial(
        ShaderID shader,
        TextureID texture
    );

    void DestroyMesh(
        MeshID id
    );

    Mesh& GetMesh(
        MeshID id
    );

    Texture& GetTexture(
        TextureID id
    );

    Shader& GetShader(
        ShaderID id
    );

    Material& GetMaterial(
        MaterialID id
    );

private:

    std::unordered_map<
        uint32_t,
        std::unique_ptr<Mesh>
    > _meshes;

    std::unordered_map<
        uint32_t,
        std::unique_ptr<Texture>
    > _textures;

    std::unordered_map<
        uint32_t,
        std::unique_ptr<Shader>
    > _shaders;

    std::unordered_map<
        uint32_t,
        std::unique_ptr<Material>
    > _materials;

    uint32_t _nextMeshID = 0;
    uint32_t _nextMaterialID = 0;
    uint32_t _nextTextureID = 0;
    uint32_t _nextShaderID = 0;
};