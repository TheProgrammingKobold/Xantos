#pragma once

#include "../Core/RenderResourceQueue.h"
#include "../Core/TerrainWorld.h"
#include "../Core/Frustum.h"

#include "../Render/RenderPacket.h"

#include "../Game/Entity/Entity.h"

#include <memory>
#include <vector>
#include <cstdint>

class Camera;

class Scene
{
public:

    Scene(
        MaterialID terrainMaterial,
        RenderResourceQueue& resourceQueue
    )
        : _world(
            std::make_unique<TerrainWorld>(
                terrainMaterial,
                resourceQueue
            )
        )
    {
    }

    template<typename T = Entity>
    T& CreateEntity()
    {
        auto entity =
            std::make_unique<T>();

        T& reference =
            *entity;

        _entities.push_back(
            std::move(entity)
        );

        return reference;
    }

    void Update(
        float deltaTime
    );

    void UpdateTerrain(
        float playerX,
        float playerZ,
        uint64_t safeAfterFrame
    );

    void PhysicsUpdate(
        float fixedDeltaTime
    );

    void BuildRenderPacket(
        RenderPacket& packet,
        const Camera& camera
    );

private:

    std::vector<
        std::unique_ptr<Entity>
    > _entities;

    std::unique_ptr<TerrainWorld>
        _world;

    Frustum _frustum;
};