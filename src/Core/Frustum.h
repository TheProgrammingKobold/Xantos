#pragma once

#include <GLM/glm.hpp>
#include "AABB.h"

class Frustum
{
public:
    void Update(const glm::mat4& matrix);

    bool Intersects(const AABB& bounds) const;

private:
    glm::vec4 _planes[6];
};