#include "Renderer.h"

#include "../Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <stdexcept>
#include <utility>

Renderer::Renderer(
    Window& window
)
    : _window(window),
    _camera(
        window.GetWidth(),
        window.GetHeight(),
        glm::vec3(
            0.0f,
            0.0f,
            2.0f
        )
    )
{
    GLFWwindow* nativeWindow =
        _window.GetNativeWindow();

    glfwMakeContextCurrent(
        nativeWindow
    );

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

    glfwSwapInterval(0);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    _viewportWidth =
        _window.GetWidth();

    _viewportHeight =
        _window.GetHeight();

    glViewport(
        0,
        0,
        _viewportWidth,
        _viewportHeight
    );

    _textRenderer =
        std::make_unique<TextRenderer>(
            "Assets/Fonts/SpaceMono-Regular.ttf",
            256,
            400
        );

    _textRenderer->SetProjection(
        _camera.GetOrthoProjection()
    );

    _textRenderer->SetViewportSize(
        _viewportWidth,
        _viewportHeight
    );

    _skyboxRenderer =
        std::make_unique<SkyboxRenderer>();

    std::array<std::string, 6> faces =
    {
        "Assets/Textures/Cubemap/bkg1_right.png",
        "Assets/Textures/Cubemap/bkg1_left.png",
        "Assets/Textures/Cubemap/bkg1_top.png",
        "Assets/Textures/Cubemap/bkg1_bot.png",
        "Assets/Textures/Cubemap/bkg1_front.png",
        "Assets/Textures/Cubemap/bkg1_back.png"
    };

    _skybox =
        std::make_unique<Cubemap>(
            faces
        );
}

void Renderer::ProcessResourceRequests(
    RenderResourceQueue& resourceQueue,
    std::size_t maxMeshUploadsPerFrame
)
{
    MeshReleaseRequest releaseRequest;

    while (
        resourceQueue.TryGetMeshRelease(
            releaseRequest
        )
        )
    {
        _assetManager.DestroyMesh(
            releaseRequest.mesh
        );
    }

    for (
        std::size_t i = 0;
        i < maxMeshUploadsPerFrame;
        ++i
        )
    {
        MeshUploadRequest uploadRequest;

        if (
            !resourceQueue.TryGetMeshUpload(
                uploadRequest
            )
            )
        {
            break;
        }

        MeshID mesh =
            _assetManager.LoadMesh(
                uploadRequest.vertices,
                uploadRequest.indices
            );

        resourceQueue.SubmitMeshUploadResult({
            uploadRequest.requestID,
            uploadRequest.chunkX,
            uploadRequest.chunkZ,
            mesh
            });
    }
}

void Renderer::Render(
    const RenderPacket& packet
)
{
    UpdateCamera(
        packet.camera
    );

    BeginFrame();

    RenderSkybox();

    Render3D(packet);

    RenderText(packet);

    EndFrame();
}

void Renderer::UpdateCamera(
    const RenderCameraState& state
)
{
    _camera.SetWidthHeight(
        state.width,
        state.height
    );

    _camera.SetPosition(
        state.position
    );

    _camera.Rotate(
        state.yaw,
        state.pitch
    );

    if (
        state.width != _viewportWidth ||
        state.height != _viewportHeight
        )
    {
        _viewportWidth =
            state.width;

        _viewportHeight =
            state.height;

        glViewport(
            0,
            0,
            _viewportWidth,
            _viewportHeight
        );

        _textRenderer->SetProjection(
            _camera.GetOrthoProjection()
        );

        _textRenderer->SetViewportSize(
            _viewportWidth,
            _viewportHeight
        );
    }
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

void Renderer::Render3D(
    const RenderPacket& packet
)
{
    for (
        const auto& command :
        packet.drawCommands
        )
    {
        if (
            !command.mesh ||
            !command.material
            )
        {
            continue;
        }

        Material& material =
            _assetManager.GetMaterial(
                command.material
            );

        Shader& shader =
            material.GetShader(
                _assetManager
            );

        Mesh& mesh =
            _assetManager.GetMesh(
                command.mesh
            );

        material.Bind(
            _assetManager
        );

        shader.SetMatrix(
            _camera.GetMatrix(),
            "camMatrix"
        );

        shader.SetMatrix(
            command.transform,
            "model"
        );

        // Your Mesh class needs to expose Draw().
        mesh.Draw();
    }
}

void Renderer::RenderText(
    const RenderPacket& packet
)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    for (
        const auto& command :
        packet.textCommands
        )
    {
        _textRenderer->RenderText(
            command.text,
            command.position.x,
            command.position.y,
            command.scale,
            command.color
        );
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Renderer::RenderSkybox()
{
    _skyboxRenderer->Render(
        _camera,
        *_skybox
    );
}

void Renderer::EndFrame()
{
    _window.SwapBuffers();
}