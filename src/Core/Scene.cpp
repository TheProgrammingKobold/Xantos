#include "Scene.h"

Entity& Scene::CreateEntity()
{
    _entities.emplace_back();

    return _entities.back();
}

void Scene::Update(float deltaTime)
{
    for (auto& entity : _entities)
    {
        entity.transform.rotation.y += deltaTime;
        entity.transform.rotation.z += deltaTime * 0.5f;
    }
}

void Scene::Render(Renderer& renderer)
{
    for (const auto& entity : _entities)
    {
        if (!entity.mesh || !entity.material)
            continue;

        renderer.Submit({
            entity.mesh,
            entity.material,
            entity.transform.GetMatrix()
            });
    }
}
