#pragma once

#include "TerrainChunk.h"
#include "TerrainGenerator.h"
#include <map>

class TerrainWorld
{
public:

    TerrainWorld(std::shared_ptr<Material> material);

    void AddTerrainChunk(int chunkX, int chunkZ, std::unique_ptr<TerrainChunk> terrainChunk)
    {
        _terrainChunks.emplace(std::make_pair(chunkX, chunkZ), std::move(terrainChunk));
    }

    bool HasTerrainChunk(int chunkX, int chunkZ) const
    {
        return _terrainChunks.contains({ chunkX, chunkZ });
    }

    const auto& GetTerrainChunks() const
    {
        return _terrainChunks;
    }

    void UpdateTerrain(float playerX, float playerZ);

    void RemoveTerrainChunksOutsideRadius(int centerChunkX, int centerChunkZ, int radius);

    TerrainGenerator GetTerrainGenerator() const { return _terrainGenerator; }

private:

    TerrainGenerator _terrainGenerator;

    std::shared_ptr<Material> _material;

    std::map<std::pair<int, int>, std::unique_ptr<TerrainChunk>> _terrainChunks;

    const int _chunkSize = TerrainChunk::GetChunkSize();
    const int _loadRadius = 20;
    const int _unloadRadius = _loadRadius + 1;
};