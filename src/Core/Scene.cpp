#include "Scene.h"
#include "../Render/Camera.h"

void Scene::Update(float deltaTime)
{
    for (auto& entity : _entities)
    {
        entity->Update(deltaTime);
    }
}

void Scene::RemoveTerrainChunksOutsideRadius(int centerChunkX, int centerChunkZ, int radius)
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

    for (const auto& terrainEntry : _terrainChunks)
	{
        const auto& terrainChunk = terrainEntry.second;
        if (_frunstum.Intersects(terrainChunk->GetChunkBounds()))
        {
            renderer.Submit(terrainChunk->GetDrawCommand());
        }
	}
}
