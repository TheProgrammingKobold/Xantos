#include "Scene.h"

void Scene::Update(float deltaTime)
{
    for (auto& entity : _entities)
    {
        entity->Update(deltaTime);
    }
}

void Scene::Render(Renderer& renderer)
{
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
}
