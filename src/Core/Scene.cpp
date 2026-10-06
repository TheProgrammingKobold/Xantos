#include "Scene.h"

#include "../Render/Camera.h"

void Scene::Update(
    float deltaTime
)
{
    for (
        auto& entity :
        _entities
        )
    {
        entity->Update(
            deltaTime
        );
    }
}

void Scene::UpdateTerrain(
    float playerX,
    float playerZ,
    uint64_t safeAfterFrame
)
{
    _world->UpdateTerrain(
        playerX,
        playerZ,
        safeAfterFrame
    );
}

void Scene::PhysicsUpdate(
    float fixedDeltaTime
)
{
    for (
        auto& entity :
        _entities
        )
    {
        entity->PhysicsUpdate(
            fixedDeltaTime,
            _world->GetTerrainGenerator()
        );
    }
}

void Scene::BuildRenderPacket(
    RenderPacket& packet,
    const Camera& camera
)
{
    const glm::mat4 cameraMatrix =
        camera.GetPerspectiveProjection() *
        camera.GetViewMatrix();

    _frustum.Update(
        cameraMatrix
    );

    for (
        const auto& entity :
        _entities
        )
    {
        if (
            !entity->mesh ||
            !entity->material
            )
        {
            continue;
        }

        packet.drawCommands.push_back({
            entity->mesh,
            entity->material,
            entity->transform.GetMatrix()
            });
    }

    for (
        const auto& [chunkCoord, terrainChunk] :
        _world->GetTerrainChunks()
        )
    {
        if (
            !terrainChunk->HasMesh()
            )
        {
            continue;
        }

        if (
            _frustum.Intersects(
                terrainChunk
                ->GetChunkBounds()
            )
            )
        {
            packet.drawCommands.push_back(
                terrainChunk
                ->GetDrawCommand()
            );
        }
    }
}