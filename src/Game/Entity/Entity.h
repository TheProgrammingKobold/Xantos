#pragma once

#include "../../Core/Transform.h"

#include <memory>

class Mesh;
class Material;
class TerrainGenerator;

class Entity
{
public:
    virtual ~Entity() = default;

    virtual void Update(float deltaTime) {}
    virtual void PhysicsUpdate(float fixedDeltaTime, const TerrainGenerator& terrainGenerator) {}


    Transform transform;

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};