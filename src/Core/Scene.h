#pragma once

#include "../Render/Renderer.h"
#include "../Game/Entity/Entity.h"
#include "../Core/TerrainChunk.h"
#include "../Core/Frustum.h"
#include <map>

class Scene
{
public:

    template<typename T = Entity>
    T& CreateEntity()
    {
        auto entity = std::make_unique<T>();

        T& reference = *entity;

        _entities.push_back(std::move(entity));

        return reference;
    }

    void AddTerrainChunk(int chunkX, int chunkZ, std::unique_ptr<TerrainChunk> terrainChunk)
	{
        _terrainChunks.emplace(std::make_pair(chunkX, chunkZ), std::move(terrainChunk));
	}

    bool HasTerrainChunk(int chunkX, int chunkZ) const
    {
        return _terrainChunks.contains({ chunkX, chunkZ });
    }

    void RemoveTerrainChunksOutsideRadius(int centerChunkX, int centerChunkZ, int radius);

    void Update(float deltaTime);
    void Render(Renderer& renderer);

private:
    std::vector<std::unique_ptr<Entity>> _entities;
    std::map<std::pair<int, int>, std::unique_ptr<TerrainChunk>> _terrainChunks;
    Frustum _frunstum;
};