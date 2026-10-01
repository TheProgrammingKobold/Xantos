#include "TerrainWorld.h"

TerrainWorld::TerrainWorld(std::shared_ptr<Material> material)
{
    _material = material;
}

void TerrainWorld::UpdateTerrain(float playerX, float playerZ)
{
    const int playerChunkX = static_cast<int>(std::floor(playerX / _chunkSize));
    const int playerChunkZ = static_cast<int>(std::floor(playerZ / _chunkSize));

    RemoveTerrainChunksOutsideRadius(playerChunkX, playerChunkZ, _unloadRadius);

    for (int dz = -_loadRadius; dz <= _loadRadius; ++dz)
    {
        for (int dx = -_loadRadius; dx <= _loadRadius; ++dx)
        {
            if (dx * dx + dz * dz > _loadRadius * _loadRadius)
                continue;

            const int chunkX = playerChunkX + dx;
            const int chunkZ = playerChunkZ + dz;
            if (!HasTerrainChunk(chunkX, chunkZ))
            {
                auto chunk = std::make_unique<TerrainChunk>(_terrainGenerator, _material, chunkX, chunkZ);
                AddTerrainChunk(chunkX, chunkZ, std::move(chunk));
            }
        }
    }
}

void TerrainWorld::RemoveTerrainChunksOutsideRadius(int centerChunkX, int centerChunkZ, int radius)
{
    const int radiusSquared = radius * radius;
    for (auto chunk = _terrainChunks.begin(); chunk != _terrainChunks.end();)
    {
        const int dx = chunk->first.first - centerChunkX;
        const int dz = chunk->first.second - centerChunkZ;
        if (dx * dx + dz * dz > radiusSquared)
            chunk = _terrainChunks.erase(chunk);
        else
            ++chunk;
    }
}
