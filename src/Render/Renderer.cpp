#include "Renderer.h"

#include "../Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <stdexcept>

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
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

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

    if (_viewportWidth <= 0)
        _viewportWidth = 1;

    if (_viewportHeight <= 0)
        _viewportHeight = 1;

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

    _ssao =
        std::make_unique<SSAO>(
            _viewportWidth,
            _viewportHeight
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
        _pendingMeshReleases.push_back(
            releaseRequest
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

    for (
        auto it = _pendingMeshReleases.begin();
        it != _pendingMeshReleases.end();
    )
    {
        if (
            _lastRenderedPacketFrame >=
            it->safeAfterFrame
        )
        {
            _assetManager.DestroyMesh(
                it->mesh
            );

            it =
                _pendingMeshReleases.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Renderer::Render(
    const RenderPacket& packet
)
{
    UpdateCamera(
        packet.camera
    );

    if (_viewportWidth <= 0 || _viewportHeight <= 0)
        return;

    BeginFrame();

    // IMPORTANT: keep the original 3D render path on the default
    // framebuffer. SSAO only copies the finished color/depth afterward.
    Render3D(packet);
    _ssao->CaptureScene();

    // Render the actual mesh vertex normals into a separate normal texture.
    // This is a second geometry pass, but it never replaces or modifies the
    // working scene render path above. The pass is depth-tested against the
    // captured scene depth, so only visible geometry contributes normals.
    _ssao->BeginNormalPass();

    Shader& normalShader =
        _ssao->GetNormalShader();

    normalShader.Activate();

    normalShader.SetMatrix(
        _camera.GetMatrix(),
        "camMatrix"
    );

    normalShader.SetMatrix(
        _camera.GetViewMatrix(),
        "viewMatrix"
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(
        GL_TEXTURE_2D,
        _ssao->GetSceneDepthTexture()
    );

    normalShader.SetInt(
        0,
        "sceneDepth"
    );

    normalShader.SetVec3(
        glm::vec3(
            static_cast<float>(_viewportWidth),
            static_cast<float>(_viewportHeight),
            0.0f
        ),
        "screenSize"
    );

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

        Mesh& mesh =
            _assetManager.GetMesh(
                command.mesh
            );

        normalShader.SetMatrix(
            command.transform,
            "model"
        );

        mesh.Draw();
    }

    _ssao->EndNormalPass();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate simple raw SSAO from depth + the actual mesh normals.
    // No blur and no temporal accumulation.
    _ssao->Generate(
        _camera.GetPerspectiveProjection()
    );

    // Put scene color * AO onto the default framebuffer. The original
    // scene depth is already present there, so nothing needs restoring.
    _ssao->Composite();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glActiveTexture(GL_TEXTURE0);

    RenderSkybox();
    RenderText(packet);

    EndFrame();

    _lastRenderedPacketFrame =
        packet.frameNumber;
}

void Renderer::UpdateCamera(
    const RenderCameraState& state
)
{
    _camera.SetPosition(
        state.position
    );

    _camera.Rotate(
        state.yaw,
        state.pitch
    );

    if (state.width <= 0 || state.height <= 0)
        return;

    _camera.SetWidthHeight(
        state.width,
        state.height
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

        _ssao->Resize(
            _viewportWidth,
            _viewportHeight
        );
    }
}

void Renderer::BeginFrame()
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    glViewport(
        0,
        0,
        _viewportWidth,
        _viewportHeight
    );

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    glActiveTexture(GL_TEXTURE0);

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
