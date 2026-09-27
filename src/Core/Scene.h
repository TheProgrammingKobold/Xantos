#pragma once

#include "../Render/Renderer.h"
#include "../Game/Entity/Entity.h"

class Scene
{
public:
    Entity& CreateEntity();

    void Update(float deltaTime);
    void Render(Renderer& renderer);

private:
    std::vector<Entity> _entities;
};