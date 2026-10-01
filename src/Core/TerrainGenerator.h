#pragma once

class TerrainGenerator
{
public:
    float GetHeight(int x, int z) const;
    float GetInterpolatedHeight(float x, float z) const;
};