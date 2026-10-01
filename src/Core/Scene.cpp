#include "Scene.h"
#include "../Render/Camera.h"

void Scene::Update(float deltaTime)
{
    for (auto& entity : _entities)
    {
        entity->Update(deltaTime);
    }
}

void Scene::UpdateTerrain(float playerX, float playerZ)
{
    _world->UpdateTerrain(playerX, playerZ);
}

void Scene::PhysicsUpdate(float fixedDeltaTime)
{
    for (auto& entity : _entities)
    {
        entity->PhysicsUpdate(fixedDeltaTime, _world->GetTerrainGenerator());
    }
}

void Scene::Render(Renderer& renderer)
{
    glm::mat4 matrix = renderer.GetCamera().GetPerspectiveProjection() * renderer.GetCamera().GetViewMatrix();
    _frunstum.Update(matrix);
    for (const auto& entity : _entities)
    {
        if (!entity->mesh || !entity->material)
            continue;

        renderer.Submit({
            entity->mesh,
            entity->material,
            entity->transform.GetMatrix()
            });
    }

    for (const auto& [chunkCoord, terrainChunk] : _world->GetTerrainChunks())
	{
        if (_frunstum.Intersects(terrainChunk->GetChunkBounds()))
        {
            renderer.Submit(terrainChunk->GetDrawCommand());
        }
	}
}
