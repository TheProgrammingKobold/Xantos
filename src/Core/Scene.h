#pragma once

#include "../Render/Renderer.h"
#include "../Game/Entity/Entity.h"
#include "../Core/TerrainWorld.h"
#include "../Core/Frustum.h"
#include <map>

class Scene
{
public:

    explicit Scene(std::shared_ptr<Material> terrainMaterial)
        :_world(std::make_unique<TerrainWorld>(std::move(terrainMaterial)))
    {
    }

    template<typename T = Entity>
    T& CreateEntity()
    {
        auto entity = std::make_unique<T>();

        T& reference = *entity;

        _entities.push_back(std::move(entity));

        return reference;
    }

    void Update(float deltaTime);
    void UpdateTerrain(float playerX, float playerZ);
    void PhysicsUpdate(float fixedDeltaTime);
    void Render(Renderer& renderer);

private:
    std::vector<std::unique_ptr<Entity>> _entities;
    std::unique_ptr<TerrainWorld> _world;
    Frustum _frunstum;
};