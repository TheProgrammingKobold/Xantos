#include "TerrainGenerator.h"

#include "PerlinNoise.hpp"
#include <cmath>

float TerrainGenerator::GetHeight(int x, int z) const
{
    static const siv::PerlinNoise perlin(123456789);
    constexpr double noiseScale = 0.005;
    constexpr double heightScale = 48.0;

    const double noise = perlin.normalizedOctave2D_01(
        x * noiseScale,
        z * noiseScale,
        5,
        0.5
    );

    return static_cast<float>(std::round((noise - 0.5) * heightScale));
}
