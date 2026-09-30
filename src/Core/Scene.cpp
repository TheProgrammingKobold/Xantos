#include "Scene.h"
#include "../Render/Camera.h"

void Scene::Update(float deltaTime)
{
    for (auto& entity : _entities)
    {
        entity->Update(deltaTime);
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

	for (const auto& terrainChunk : _terrainChunks)
	{
        if (_frunstum.Intersects(terrainChunk->GetChunkBounds()))
        {
            renderer.Submit(terrainChunk->GetDrawCommand());
        }
	}
}
