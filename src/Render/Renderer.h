#pragma once

#include <functional>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DrawCommand.h"
#include "TextRenderer.h"

class Camera;
class Window;

class Renderer
{
public:
    explicit Renderer(Window& window, Camera& camera);

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void BeginFrame();
    void ExecuteCommands();
    void EndFrame();

    void Submit(const DrawCommand& command);
    void Submit(const TextCommand& command);

    void Resize(int width, int height);

private:

    void Render3D();
    void RenderText();

private:
    Window& _window;
    Camera& _camera;
    std::unique_ptr<TextRenderer> _textRenderer;

    std::vector<DrawCommand> _drawCommands;
    std::vector<TextCommand> _textCommands;
};