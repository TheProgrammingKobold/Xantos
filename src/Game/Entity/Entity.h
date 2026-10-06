#pragma once

#include "../../Core/Transform.h"
#include "../../Core/RenderAssetTypes.h"

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

    MeshID mesh;
    MaterialID material;
};