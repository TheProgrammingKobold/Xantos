// Window.h
#pragma once

#include <string>
#include <atomic>
#include <GLFW/glfw3.h>

class EventBus;

class Window
{
public:
    Window(int width, int height, const std::string& title, EventBus& events);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void PollEvents();
    void SwapBuffers();

    void SetMouseCaptured(bool captured);
    void ApplyPendingMouseCapture();

    bool ShouldClose() const;
    void Close();

    int GetWidth() const;
    int GetHeight() const;

    GLFWwindow* GetNativeWindow() const;

private:
    GLFWwindow* _window = nullptr;
    std::atomic<bool> _requestedMouseCaptured = false;
    bool _mouseCaptured = false;

    int _width;
    int _height;

    EventBus& _events;

    static void FramebufferSizeCallback(
        GLFWwindow* window,
        int width,
        int height
    );

    static void KeyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods
    );

    static void CursorPositionCallback(GLFWwindow* window, double x, double y);
    
    static void MouseScrolledCallback(GLFWwindow* window, double xoffset, double yoffset);
};