#pragma once

#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>

class SSAO
{
public:

    static constexpr int KERNEL_SIZE = 64;
    static constexpr int NOISE_SIZE = 4;

    static constexpr float DEFAULT_RADIUS = 0.70f;
    static constexpr float DEFAULT_BIAS = 0.025f;

    SSAO(int width, int height);
    ~SSAO();

    SSAO(const SSAO&) = delete;
    SSAO& operator=(const SSAO&) = delete;

    void Resize(int width, int height);

    // Copies the finished scene from the default framebuffer.
    // The copied depth is also used as the depth attachment for the
    // normal pass, so normals are generated only for actually visible
    // scene geometry.
    void CaptureScene();

    // Starts a second, depth-tested pass that writes the actual mesh
    // vertex normals transformed into view space. This pass does not
    // touch the default framebuffer.
    void BeginNormalPass();
    void EndNormalPass();

    Shader& GetNormalShader() noexcept
    {
        return _normalShader;
    }

    GLuint GetSceneDepthTexture() const noexcept
    {
        return _sceneDepth;
    }

    // Generates raw SSAO from scene depth + the normal texture.
    // There is intentionally no blur or temporal accumulation.
    void Generate(const glm::mat4& projection);

    // Draws scene color * AO to the default framebuffer.
    void Composite();

private:

    void CreateScreenQuad();
    void DestroyScreenQuad();

    void CreateNoiseTexture();
    void DestroyNoiseTexture();

    void CreateFramebuffers(int width, int height);
    void DestroyFramebuffers();

    void CheckFramebuffer(
        GLuint framebuffer,
        const char* name
    ) const;

private:

    int _width = 1;
    int _height = 1;

    // Captured scene color/depth.
    GLuint _sceneFBO = 0;
    GLuint _sceneColor = 0;
    GLuint _sceneDepth = 0;

    // Actual view-space mesh normals. Reuses _sceneDepth as its depth
    // attachment so only visible geometry writes a normal.
    GLuint _normalFBO = 0;
    GLuint _normalColor = 0;

    // Raw AO target.
    GLuint _ssaoFBO = 0;
    GLuint _ssaoColor = 0;

    GLuint _noiseTexture = 0;

    GLuint _screenVAO = 0;
    GLuint _screenVBO = 0;

    std::array<glm::vec3, KERNEL_SIZE> _kernel{};

    Shader _normalShader;
    Shader _ssaoShader;
    Shader _compositeShader;
};
