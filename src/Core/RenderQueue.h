#pragma once

#include "../Render/RenderPacket.h"

#include <mutex>
#include <utility>

class RenderQueue
{
public:

    void Submit(RenderPacket packet)
    {
        std::lock_guard lock(_mutex);

        _pending = std::move(packet);
        _hasPending = true;
    }

    bool TryConsume(RenderPacket& packet)
    {
        std::lock_guard lock(_mutex);

        if (!_hasPending)
            return false;

        packet = std::move(_pending);
        _hasPending = false;

        return true;
    }

private:

    std::mutex _mutex;

    RenderPacket _pending;
    bool _hasPending = false;
};