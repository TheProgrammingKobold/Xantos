#pragma once

#include "Event.h"

class WindowCloseEvent : public Event
{
public:

    std::type_index GetType() const override
    {
        return typeid(WindowCloseEvent);
    }
};


class WindowResizeEvent : public Event
{
public:

    WindowResizeEvent(int width, int height)
        : width(width),
        height(height)
    {
    }

    int width;
    int height;

    std::type_index GetType() const override
    {
        return typeid(WindowResizeEvent);
    }
};

class KeyPressedEvent : public Event
{
public:

    KeyPressedEvent(int key, int scancode, int mods)
        : key(key),
        scancode(scancode),
        mods(mods)
    {
    }

    int key;
    int scancode;
    int mods;

    std::type_index GetType() const override
    {
        return typeid(KeyPressedEvent);
    }
};

class KeyRepeatEvent : public Event
{
public:
    KeyRepeatEvent(int key, int scancode, int mods)
        : key(key),
        scancode(scancode),
        mods(mods)
    {
    }

    int key;
    int scancode;
    int mods;

    std::type_index GetType() const override
    {
        return typeid(KeyRepeatEvent);
    }
};

class KeyReleasedEvent : public Event
{
public:

    KeyReleasedEvent(int key, int scancode, int mods)
        : key(key),
        scancode(scancode),
        mods(mods)
    {
    }

    int key;
    int scancode;
    int mods;

    std::type_index GetType() const override
    {
        return typeid(KeyReleasedEvent);
    }
};

class MouseMovedEvent : public Event
{
public:

    MouseMovedEvent(double x, double y)
        : x(x),
        y(y)
    {
    }

    double x;
    double y;

    std::type_index GetType() const override
    {
        return typeid(MouseMovedEvent);
    }
};


class MouseButtonPressedEvent : public Event
{
public:

    MouseButtonPressedEvent(int button, int mods)
        : button(button),
        mods(mods)
    {
    }

    int button;
    int mods;

    std::type_index GetType() const override
    {
        return typeid(MouseButtonPressedEvent);
    }
};


class MouseButtonReleasedEvent : public Event
{
public:

    MouseButtonReleasedEvent(int button, int mods)
        : button(button),
        mods(mods)
    {
    }

    int button;
    int mods;

    std::type_index GetType() const override
    {
        return typeid(MouseButtonReleasedEvent);
    }
};

class MouseCaptureChangedEvent : public Event
{
public:

    MouseCaptureChangedEvent(bool captured)
        : captured(captured)
    {
    }

    bool captured;

    std::type_index GetType() const override
    {
        return typeid(MouseCaptureChangedEvent);
    }
};