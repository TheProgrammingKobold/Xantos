#include "Renderer.h"

#include "../Core/Window.h"
#include "Camera.h"

#include <stdexcept>
#include <utility>

Renderer::Renderer(Window& window, Camera& camera)
    : _window(window),
      _camera(camera)
{
    GLFWwindow* nativeWindow =
        _window.GetNativeWindow();

    glfwMakeContextCurrent(nativeWindow);

    if (!gladLoadGLLoader(
        reinterpret_cast<GLADloadproc>(
            glfwGetProcAddress
            )
    ))
    {
        throw std::runtime_error(
            "Failed to initialize GLAD"
        );
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glViewport(
        0,
        0,
        _window.GetWidth(),
        _window.GetHeight()
    );

    _textRenderer = std::make_unique<TextRenderer>("Assets/Fonts/SpaceMono-Regular.ttf", 256, 400);
    _textRenderer->SetProjection(_camera.getOrthoProjection());
    _textRenderer->SetViewportSize(_window.GetWidth(), _window.GetHeight());
}

void Renderer::BeginFrame()
{
    glClearColor(
        0.01f,
        0.01f,
        0.01f,
        1.0f
    );

    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );
}

void Renderer::ExecuteCommands()
{
    Render3D();
    RenderText();
}

void Renderer::Render3D()
{
    for (const auto& command : _drawCommands)
    {
        command.material->Bind();

        auto& shader = command.material->GetShader();

        shader.setMatrix(_camera.getMatrix(), "camMatrix");
        shader.setMatrix(command.transform, "model");


        command.mesh->Draw();
    }

    _drawCommands.clear();
}

void Renderer::RenderText()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    for (const auto& command : _textCommands)
    {
        _textRenderer->RenderText(
            command.text,
            command.position.x,
            command.position.y,
            command.scale,
            command.color
        );
    }

    _textCommands.clear();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Renderer::EndFrame()
{
    _window.SwapBuffers();
}

void Renderer::Resize(int width, int height)
{
    glViewport(0, 0, width, height);
    _camera.setWidthHeight(width, height);

    _textRenderer->SetProjection(_camera.getOrthoProjection());
    _textRenderer->SetViewportSize(width, height);
}

void Renderer::Submit(const DrawCommand& command)
{
    _drawCommands.push_back(std::move(command));
}

void Renderer::Submit(const TextCommand& command)
{
    _textCommands.push_back(std::move(command));
}