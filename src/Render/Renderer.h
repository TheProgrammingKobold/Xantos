#pragma once

#include "../Core/RenderAssetManager.h"
#include "../Core/RenderResourceQueue.h"

#include "RenderPacket.h"
#include "TextRenderer.h"
#include "SkyboxRenderer.h"
#include "Camera.h"
#include "SSAO.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstddef>
#include <memory>
#include <cstdint>
#include <vector>

class Window;

class Renderer
{
public:

    explicit Renderer(
        Window& window
    );

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Render(
        const RenderPacket& packet
    );

    void ProcessResourceRequests(
        RenderResourceQueue& resourceQueue,
        std::size_t maxMeshUploadsPerFrame = 4
    );

    RenderAssetManager& GetAssetManager() noexcept
    {
        return _assetManager;
    }

private:

    void BeginFrame();
    void EndFrame();

    void UpdateCamera(
        const RenderCameraState& state
    );

    void Render3D(
        const RenderPacket& packet
    );

    void RenderText(
        const RenderPacket& packet
    );

    void RenderSkybox();

private:

    Window& _window;

    Camera _camera;

    std::unique_ptr<TextRenderer>
        _textRenderer;

    std::unique_ptr<SkyboxRenderer>
        _skyboxRenderer;

    std::unique_ptr<Cubemap>
        _skybox;

    std::unique_ptr<SSAO>
        _ssao;

    RenderAssetManager _assetManager;

    int _viewportWidth = 0;
    int _viewportHeight = 0;

    uint64_t _lastRenderedPacketFrame = 0;
    std::vector<MeshReleaseRequest>
        _pendingMeshReleases;
};
