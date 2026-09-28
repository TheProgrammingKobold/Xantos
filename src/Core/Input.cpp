#include "Input.h"



bool Input::_keys[GLFW_KEY_LAST + 1] = {};
glm::vec2 Input::_mousePosition = {};
glm::vec2 Input::_mouseDelta = {};
bool Input::_mouseCaptured = false;

void Input::Initialize(EventBus& events)
{
    events.Subscribe<KeyPressedEvent>(
        [&events](const KeyPressedEvent& event)
        {
            _keys[event.key] = true;
            if (event.key == GLFW_KEY_E)
            {
                SetMouseCaptured(!_mouseCaptured);
                events.Post<MouseCaptureChangedEvent>(_mouseCaptured);
            }
        }
    );

    events.Subscribe<KeyReleasedEvent>(
        [](const KeyReleasedEvent& event)
        {
            _keys[event.key] = false;
        }
    );

    events.Subscribe<MouseMovedEvent>(
        [](const MouseMovedEvent& event)
        {
            glm::vec2 newPosition(event.x, event.y);

            Input::_mouseDelta +=
                newPosition - Input::_mousePosition;

            Input::_mousePosition = newPosition;
        }
    );
}

bool Input::IsKeyDown(int key)
{
    if (key < 0 || key > GLFW_KEY_LAST)
        return false;

    return _keys[key];
}

bool Input::IsMouseCaptured()
{
    return _mouseCaptured;
}

void Input::SetMouseCaptured(bool captured)
{
    _mouseCaptured = captured;
}

void Input::SetKeyState(int key, bool down)
{
    if (key < 0 || key > GLFW_KEY_LAST)
        return;

    _keys[key] = down;
}

void Input::EndFrame()
{
    _mouseDelta = glm::vec2(0.0f);
}