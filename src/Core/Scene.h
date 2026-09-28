#pragma once

#include "../Render/Renderer.h"
#include "../Game/Entity/Entity.h"

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

    void Update(float deltaTime);
    void Render(Renderer& renderer);

private:
    std::vector<std::unique_ptr<Entity>> _entities;
};