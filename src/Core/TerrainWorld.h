#pragma once

#include "TerrainChunk.h"
#include "TerrainGenerator.h"
#include "RenderResourceQueue.h"

#include <cmath>
#include <map>
#include <memory>
#include <utility>
#include <cstdint>

class TerrainWorld
{
public:

    TerrainWorld(
        MaterialID material,
        RenderResourceQueue& resourceQueue
    );

    void AddTerrainChunk(
        int chunkX,
        int chunkZ,
        std::unique_ptr<TerrainChunk> terrainChunk
    )
    {
        _terrainChunks.emplace(
            std::make_pair(
                chunkX,
                chunkZ
            ),
            std::move(terrainChunk)
        );
    }

    bool HasTerrainChunk(
        int chunkX,
        int chunkZ
    ) const
    {
        return _terrainChunks.contains({
            chunkX,
            chunkZ
            });
    }

    const auto& GetTerrainChunks() const
    {
        return _terrainChunks;
    }

    void UpdateTerrain(
        float playerX,
        float playerZ,
        uint64_t safeAfterFrame
    );

    void RemoveTerrainChunksOutsideRadius(
        int centerChunkX,
        int centerChunkZ,
        int radius,
        uint64_t safeAfterFrame
    );

    TerrainGenerator GetTerrainGenerator() const
    {
        return _terrainGenerator;
    }

private:

    void ProcessMeshUploadResults();

    void QueueMeshUpload(
        TerrainChunk& chunk,
        int chunkX,
        int chunkZ
    );

private:

    TerrainGenerator _terrainGenerator;

    MaterialID _material;

    RenderResourceQueue&
        _resourceQueue;

    std::map<
        std::pair<int, int>,
        std::unique_ptr<TerrainChunk>
    > _terrainChunks;

    const int _chunkSize =
        TerrainChunk::GetChunkSize();

    const int _loadRadius = 20 * 2;

    const int _unloadRadius =
        _loadRadius + 1;

    // CPU generation is now done on the main
    // thread, so don't generate the entire world
    // in one frame.
    static constexpr int
        MAX_NEW_CHUNKS_PER_UPDATE = 4;
};