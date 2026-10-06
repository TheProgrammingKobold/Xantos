#pragma once

#include "AABB.h"
#include "TerrainGenerator.h"

#include "../Render/Vertex.h"
#include "../Render/DrawCommand.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <utility>
#include <vector>

struct TerrainMeshData
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

class TerrainChunk
{
public:

    TerrainChunk(
        const TerrainGenerator& generator,
        MaterialID material,
        int chunkX,
        int chunkZ
    );

    void GenerateGreedyMesh(
        const TerrainGenerator& generator
    );

    void GenerateMesh(
        const TerrainGenerator& generator
    );

    TerrainMeshData TakeMeshData();

    bool HasMesh() const noexcept
    {
        return static_cast<bool>(
            _drawCommand.mesh
            );
    }

    uint64_t GetMeshRequestID() const noexcept
    {
        return _meshRequestID;
    }

    void SetMeshRequestID(
        uint64_t requestID
    ) noexcept
    {
        _meshRequestID = requestID;
    }

    void SetMeshID(
        uint64_t requestID,
        MeshID mesh
    )
    {
        if (
            requestID != _meshRequestID
            )
        {
            return;
        }

        _drawCommand.mesh = mesh;
    }

    const DrawCommand& GetDrawCommand() const noexcept
    {
        return _drawCommand;
    }

    AABB GetChunkBounds() const
    {
        return _aabb;
    }

    static int GetChunkSize()
    {
        return CHUNK_SIZE;
    }

private:

    void TransformIntoGreedyMesh(
        std::vector<Vertex>& vertices,
        std::vector<GLuint>& indices,
        const TerrainGenerator& generator
    );

private:

    TerrainMeshData _meshData;

    DrawCommand _drawCommand;

    AABB _aabb;

    uint64_t _meshRequestID = 0;

    static constexpr int CHUNK_SIZE =
        16 * 2;

    int _chunkX;
    int _chunkZ;
};