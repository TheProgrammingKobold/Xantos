// Window.h
#pragma once

#include <string>
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

    bool ShouldClose() const;
    void Close();

    int GetWidth() const;
    int GetHeight() const;

    GLFWwindow* GetNativeWindow() const;

private:
    GLFWwindow* _window = nullptr;

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
};