#pragma once

#include "../Render/Renderer.h"
#include "../Game/Entity/Entity.h"
#include "../Core/TerrainChunk.h"

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

	void AddTerrainChunk(std::unique_ptr<TerrainChunk> terrainChunk)
	{
		_terrainChunks.push_back(std::move(terrainChunk));
	}

    void Update(float deltaTime);
    void Render(Renderer& renderer);

private:
    std::vector<std::unique_ptr<Entity>> _entities;
	std::vector<std::unique_ptr<TerrainChunk>> _terrainChunks;
};