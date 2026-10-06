#pragma once

#include <cstdint>
#include <limits>

struct MeshID
{
    static constexpr uint32_t InvalidValue =
        std::numeric_limits<uint32_t>::max();

    uint32_t value = InvalidValue;

    constexpr explicit operator bool() const noexcept
    {
        return value != InvalidValue;
    }
};

struct TextureID
{
    static constexpr uint32_t InvalidValue =
        std::numeric_limits<uint32_t>::max();

    uint32_t value = InvalidValue;

    constexpr explicit operator bool() const noexcept
    {
        return value != InvalidValue;
    }
};

struct ShaderID
{
    static constexpr uint32_t InvalidValue =
        std::numeric_limits<uint32_t>::max();

    uint32_t value = InvalidValue;

    constexpr explicit operator bool() const noexcept
    {
        return value != InvalidValue;
    }
};

struct MaterialID
{
    static constexpr uint32_t InvalidValue =
        std::numeric_limits<uint32_t>::max();

    uint32_t value = InvalidValue;

    constexpr explicit operator bool() const noexcept
    {
        return value != InvalidValue;
    }
};