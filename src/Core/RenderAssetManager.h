#pragma once

#include <GLAD/glad.h>
#include <string>
#include <unordered_map>
#include "../Render/Vertex.h"
#include "../Render/Mesh.h"
#include "../Render/Material.h"
#include <vector>
#include <memory>

struct MeshID
{
    uint32_t value;
};

struct TextureID
{
    uint32_t value;
};

struct ShaderID
{
    uint32_t value;
};

class RenderAssetManager
{
public:

    MeshID LoadMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    TextureID LoadTexture(const std::string& texture);
    ShaderID LoadShader(const std::string& vertexShader, const std::string& fragmentShader);

    Mesh& GetMesh(MeshID id);
    Texture& GetTexture(TextureID id);
    Shader& GetShader(ShaderID id);

private:

    std::unordered_map<uint32_t, std::unique_ptr<Mesh>> _meshes;
    std::unordered_map<uint32_t, std::unique_ptr<Texture>> _textures;
    std::unordered_map<uint32_t, std::unique_ptr<Shader>> _shaders;

    uint32_t _nextMeshID = 0;
    uint32_t _nextMaterialID = 0;
    uint32_t _nextTextureID = 0;
    uint32_t _nextShaderID = 0;
};