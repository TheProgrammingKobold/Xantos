#pragma once

#include <functional>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window;

class Renderer
{
public:
    explicit Renderer(Window& window);

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void BeginFrame();
    void ExecuteCommands();
    void EndFrame();

    void Submit(std::function<void()> command);

private:
    Window& _window;

    std::vector<std::function<void()>> _commands;
};