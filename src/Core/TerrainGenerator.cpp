#include "TerrainGenerator.h"

#include "PerlinNoise.hpp"
#include <algorithm>
#include <cmath>

TerrainGenerator::TerrainGenerator(int width, int depth)
	: _width(width), _depth(depth)
{
	_heights.resize(width * depth);
}

void TerrainGenerator::Generate()
{
	const siv::PerlinNoise::seed_type seed = 123456789; // You can use any seed value you want
	const siv::PerlinNoise perlin(seed);
    constexpr double noiseScale = 0.005;
    constexpr double heightScale = 48.0;

    for (int x = 0; x < _width; ++x)
    {
        for (int z = 0; z < _depth; ++z)
        {
            double noise = perlin.normalizedOctave2D_01(
                x * noiseScale,
                z * noiseScale,
                5,
                0.5
            );

            float height = static_cast<float>(std::round((noise - 0.5) * heightScale));

            _heights[x + z * _width] = height;
        }
    }
}

float TerrainGenerator::GetHeight(int x, int z) const
{
    x = std::clamp(x, 0, _width - 1);
    z = std::clamp(z, 0, _depth - 1);
	return _heights[x + z * _width];
}
