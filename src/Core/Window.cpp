#include "Window.h"

#include "../Events/EventBus.h"
#include "../Events/EventTypes.h"

#include <stdexcept>

Window::Window(
    int width,
    int height,
    const std::string& title,
    EventBus& events
)
    : _width(width),
    _height(height),
    _events(events)
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(
        width,
        height,
        title.c_str(),
        nullptr,
        nullptr
    );

    if (!_window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(_window, this);

    glfwSetFramebufferSizeCallback(
        _window,
        FramebufferSizeCallback
    );

    glfwSetKeyCallback(
        _window,
        KeyCallback
    );

    glfwSetCursorPosCallback(_window, CursorPositionCallback);
}

Window::~Window()
{
    if (_window)
        glfwDestroyWindow(_window);

    glfwTerminate();
}

void Window::FramebufferSizeCallback(
    GLFWwindow* window,
    int width,
    int height
)
{
    auto* self =
        static_cast<Window*>(glfwGetWindowUserPointer(window));

    self->_width = width;
    self->_height = height;

    self->_events.Post<WindowResizeEvent>(
        width,
        height
    );
}

void Window::KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
)
{
    auto* self =
        static_cast<Window*>(glfwGetWindowUserPointer(window));

    switch (action)
    {
    case GLFW_PRESS:
        self->_events.Post<KeyPressedEvent>(
            key, scancode, mods
        );
        break;

    case GLFW_REPEAT:
        self->_events.Post<KeyRepeatEvent>(
            key, scancode, mods
        );
        break;

    case GLFW_RELEASE:
        self->_events.Post<KeyReleasedEvent>(
            key, scancode, mods
        );
        break;
    }
}

void Window::CursorPositionCallback(
    GLFWwindow* window,
    double x,
    double y
)
{
    auto* self =
        static_cast<Window*>(glfwGetWindowUserPointer(window));

    self->_events.Post<MouseMovedEvent>(
        static_cast<float>(x),
        static_cast<float>(y)
    );
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(_window);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(_window);
}

void Window::Close()
{
    glfwSetWindowShouldClose(_window, GLFW_TRUE);
}

int Window::GetWidth() const
{
    return _width;
}

int Window::GetHeight() const
{
    return _height;
}

GLFWwindow* Window::GetNativeWindow() const
{
    return _window;
}