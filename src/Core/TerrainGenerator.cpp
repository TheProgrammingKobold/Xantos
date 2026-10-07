#include "TerrainGenerator.h"

#include "PerlinNoise.hpp"
#include <cmath>

float TerrainGenerator::GetHeight(int x, int z) const
{
    static const siv::PerlinNoise perlin(123456789);
    constexpr double noiseScale = 0.002;
    constexpr double heightScale = 480.0 * 2;

    const double noise = perlin.normalizedOctave2D_01(
        x * noiseScale,
        z * noiseScale,
        5,
        0.5
    );

    return static_cast<float>(std::round((noise - 0.5) * heightScale));
}

float TerrainGenerator::GetInterpolatedHeight(float x, float z) const
{
    const int x0 = static_cast<int>(std::floor(x));
    const int z0 = static_cast<int>(std::floor(z));

    const int x1 = x0 + 1;
    const int z1 = z0 + 1;

    // How far we are between the integer points.
    const float tx = x - static_cast<float>(x0);
    const float tz = z - static_cast<float>(z0);

    // Sample the four surrounding points.
    const float h00 = GetHeight(x0, z0);
    const float h10 = GetHeight(x1, z0);
    const float h01 = GetHeight(x0, z1);
    const float h11 = GetHeight(x1, z1);

    // Interpolate along X.
    const float hx0 = h00 + (h10 - h00) * tx;
    const float hx1 = h01 + (h11 - h01) * tx;

    // Interpolate along Z.
    return hx0 + (hx1 - hx0) * tz;
}

glm::vec3 TerrainGenerator::GetInterpolatedNormal(
    float x,
    float z
) const
{
    constexpr float e = 0.1f;

    const float hL =
        GetInterpolatedHeight(x - e, z);

    const float hR =
        GetInterpolatedHeight(x + e, z);

    const float hD =
        GetInterpolatedHeight(x, z - e);

    const float hU =
        GetInterpolatedHeight(x, z + e);

    glm::vec3 normal(
        hL - hR,
        2.0f * e,
        hD - hU
    );

    return glm::normalize(normal);
}