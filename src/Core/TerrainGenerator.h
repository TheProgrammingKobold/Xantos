#pragma once

#include <vector>

class TerrainGenerator
{
public:
    TerrainGenerator(int width, int depth);

    void Generate();

    int GetHeight(int x, int z) const;

private:
    int _width;
    int _depth;

    std::vector<int> _heights;
};