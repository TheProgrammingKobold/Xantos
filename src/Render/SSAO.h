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

    static constexpr float DEFAULT_RADIUS = 10.75f;
    static constexpr float DEFAULT_BIAS = 0.00005f;

    SSAO(int width, int height);
    ~SSAO();

    SSAO(const SSAO&) = delete;
    SSAO& operator=(const SSAO&) = delete;

    void Resize(int width, int height);

    // Copies the already-rendered default framebuffer into the SSAO
    // color/depth textures. This deliberately does not render geometry
    // into an alternate framebuffer.
    void CaptureScene();

    // Generates raw SSAO from the scene depth texture. No blur.
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

    GLuint _sceneFBO = 0;
    GLuint _sceneColor = 0;
    GLuint _sceneDepth = 0;

    GLuint _ssaoFBO = 0;
    GLuint _ssaoColor = 0;

    GLuint _noiseTexture = 0;

    GLuint _screenVAO = 0;
    GLuint _screenVBO = 0;

    std::array<glm::vec3, KERNEL_SIZE> _kernel{};

    Shader _ssaoShader;
    Shader _compositeShader;
};
