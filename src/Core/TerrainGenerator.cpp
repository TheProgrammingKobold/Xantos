#include "TerrainGenerator.h"

#include "PerlinNoise.hpp"

TerrainGenerator::TerrainGenerator(int width, int depth)
	: _width(width), _depth(depth)
{
	_heights.resize(width * depth);
}

void TerrainGenerator::Generate()
{
	const siv::PerlinNoise::seed_type seed = 123456789; // You can use any seed value you want
	const siv::PerlinNoise perlin(seed);

    for (int x = 0; x < _width; ++x)
    {
        for (int z = 0; z < _depth; ++z)
        {
            double noise = perlin.normalizedOctave2D_01(
                x * 0.05,
                z * 0.05,
                3,
                0.5
            );

            int height = static_cast<int>(noise * 4.0);

            _heights[x + z * _width] = height;
        }
    }
}

int TerrainGenerator::GetHeight(int x, int z) const
{
	return _heights[x + z * _width];
}
