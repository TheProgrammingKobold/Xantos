#pragma once

#include <GLFW/glfw3.h>

#include "../Events/EventBus.h"
#include "../Events/EventTypes.h"

#include <glm/glm.hpp>

class Input
{
public:
    static void Initialize(EventBus& events);

    static bool IsKeyDown(int key);
    static bool IsKeyPressed(int key);
    static bool IsKeyReleased(int key);

    static glm::vec2 GetMousePosition() { return _mousePosition; }
    static glm::vec2 GetMouseDelta() { return _mouseDelta; }

    static bool IsMouseCaptured();
    static void SetMouseCaptured(bool captured);

    static void SetKeyState(int key, bool down);

    static void EndFrame();

private:
    static bool _keys[GLFW_KEY_LAST + 1];

    static glm::vec2 _mousePosition;
    static glm::vec2 _mouseDelta;

    static bool _mouseCaptured;
};