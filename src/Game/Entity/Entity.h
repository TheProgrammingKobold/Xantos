#pragma once

#include "../../Core/Transform.h"

#include <memory>

class Mesh;
class Material;

class Entity
{
public:
    Transform transform;

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};