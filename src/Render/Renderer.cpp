#include "Renderer.h"

#include "../Core/Window.h"



#include <stdexcept>
#include <utility>

Renderer::Renderer(Window& window)
    : _window(window)
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
    for (auto& command : _commands)
    {
        command();
    }

    _commands.clear();
}

void Renderer::EndFrame()
{
    _window.SwapBuffers();
}

void Renderer::Submit(std::function<void()> command)
{
    _commands.push_back(std::move(command));
}