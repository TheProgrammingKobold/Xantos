#include "TerrainWorld.h"

#include <algorithm>
#include <vector>

TerrainWorld::TerrainWorld(
    MaterialID material,
    RenderResourceQueue& resourceQueue
)
    : _material(material),
    _resourceQueue(resourceQueue)
{
}

void TerrainWorld::ProcessMeshUploadResults()
{
    MeshUploadResult result;

    while (
        _resourceQueue.TryGetMeshUploadResult(
            result
        )
        )
    {
        const auto key =
            std::make_pair(
                result.chunkX,
                result.chunkZ
            );

        auto it =
            _terrainChunks.find(key);

        // The chunk was unloaded before the GPU
        // upload finished.
        if (
            it == _terrainChunks.end()
            )
        {
            _resourceQueue.SubmitMeshRelease({
                result.mesh
                });

            continue;
        }

        TerrainChunk& chunk =
            *it->second;

        // This can happen if a chunk was unloaded
        // and then recreated before the old upload
        // finished.
        if (
            chunk.GetMeshRequestID() !=
            result.requestID
            )
        {
            _resourceQueue.SubmitMeshRelease({
                result.mesh
                });

            continue;
        }

        chunk.SetMeshID(
            result.requestID,
            result.mesh
        );
    }
}

void TerrainWorld::QueueMeshUpload(
    TerrainChunk& chunk,
    int chunkX,
    int chunkZ
)
{
    TerrainMeshData meshData =
        chunk.TakeMeshData();

    const uint64_t requestID =
        _resourceQueue.NextRequestID();

    chunk.SetMeshRequestID(
        requestID
    );

    _resourceQueue.SubmitMeshUpload({
        requestID,
        chunkX,
        chunkZ,
        std::move(meshData.vertices),
        std::move(meshData.indices)
        });
}

void TerrainWorld::UpdateTerrain(
    float playerX,
    float playerZ,
    uint64_t safeAfterFrame
)
{
    ProcessMeshUploadResults();

    const int playerChunkX =
        static_cast<int>(
            std::floor(
                playerX /
                _chunkSize
            )
            );

    const int playerChunkZ =
        static_cast<int>(
            std::floor(
                playerZ /
                _chunkSize
            )
            );

    RemoveTerrainChunksOutsideRadius(
        playerChunkX,
        playerChunkZ,
        _unloadRadius,
        safeAfterFrame
    );

    struct ChunkCandidate
    {
        int x;
        int z;
        int distanceSquared;
    };

    std::vector<ChunkCandidate>
        candidates;

    for (
        int dz = -_loadRadius;
        dz <= _loadRadius;
        ++dz
        )
    {
        for (
            int dx = -_loadRadius;
            dx <= _loadRadius;
            ++dx
            )
        {
            const int distanceSquared =
                dx * dx +
                dz * dz;

            if (
                distanceSquared >
                _loadRadius *
                _loadRadius
                )
            {
                continue;
            }

            const int chunkX =
                playerChunkX + dx;

            const int chunkZ =
                playerChunkZ + dz;

            if (
                HasTerrainChunk(
                    chunkX,
                    chunkZ
                )
                )
            {
                continue;
            }

            candidates.push_back({
                chunkX,
                chunkZ,
                distanceSquared
                });
        }
    }

    std::sort(
        candidates.begin(),
        candidates.end(),
        [](const ChunkCandidate& a,
            const ChunkCandidate& b)
        {
            return a.distanceSquared <
                b.distanceSquared;
        }
    );

    const int chunksToGenerate =
        std::min(
            static_cast<int>(
                candidates.size()
                ),
            MAX_NEW_CHUNKS_PER_UPDATE
        );

    for (
        int i = 0;
        i < chunksToGenerate;
        ++i
        )
    {
        const auto& candidate =
            candidates[i];

        auto chunk =
            std::make_unique<TerrainChunk>(
                _terrainGenerator,
                _material,
                candidate.x,
                candidate.z
            );

        TerrainChunk& chunkReference =
            *chunk;

        AddTerrainChunk(
            candidate.x,
            candidate.z,
            std::move(chunk)
        );

        QueueMeshUpload(
            chunkReference,
            candidate.x,
            candidate.z
        );
    }
}


void TerrainWorld::RemoveTerrainChunksOutsideRadius(
    int centerChunkX,
    int centerChunkZ,
    int radius,
    uint64_t safeAfterFrame
)
{
    const int radiusSquared =
        radius * radius;

    for (
        auto chunk =
        _terrainChunks.begin();

        chunk !=
        _terrainChunks.end();
        )
    {
        const int dx =
            chunk->first.first -
            centerChunkX;

        const int dz =
            chunk->first.second -
            centerChunkZ;

        if (
            dx * dx +
            dz * dz >
            radiusSquared
            )
        {
            TerrainChunk& terrainChunk =
                *chunk->second;

            if (
                terrainChunk.HasMesh()
                )
            {
                _resourceQueue.SubmitMeshRelease({
                    terrainChunk
                        .GetDrawCommand()
                        .mesh,

                    safeAfterFrame
                    });
            }

            chunk =
                _terrainChunks.erase(chunk);
        }
        else
        {
            ++chunk;
        }
    }
}