#pragma once

#include <typeindex>

class Event
{
public:
    virtual ~Event() = default;

    virtual std::type_index GetType() const = 0;
};