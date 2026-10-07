#pragma once

#include <glm/glm.hpp>

class TerrainGenerator
{
public:
    float GetHeight(int x, int z) const;
    float GetInterpolatedHeight(float x, float z) const;
    glm::vec3 GetInterpolatedNormal(float x, float z) const;
};