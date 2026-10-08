#include "SSAO.h"

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

SSAO::SSAO(
    int width,
    int height
)
    : _width(width > 0 ? width : 1),
    _height(height > 0 ? height : 1),
    _normalShader(
        "ssao_normal_vertex.shader",
        "ssao_normal_fragment.shader"
    ),
    _ssaoShader(
        "ssao_screen_vertex.shader",
        "ssao_fragment.shader"
    ),
    _compositeShader(
        "ssao_screen_vertex.shader",
        "ssao_composite_fragment.shader"
    )
{
    std::mt19937 generator(1337u);
    std::uniform_real_distribution<float> random01(0.0f, 1.0f);

    for (int i = 0; i < KERNEL_SIZE; ++i)
    {
        glm::vec3 sample(
            random01(generator) * 2.0f - 1.0f,
            random01(generator) * 2.0f - 1.0f,
            random01(generator)
        );

        sample = glm::normalize(sample);

        const float t =
            static_cast<float>(i) /
            static_cast<float>(KERNEL_SIZE);

        const float scale =
            0.10f +
            0.90f * t * t;

        _kernel[i] = sample * scale;
    }

    CreateScreenQuad();
    CreateNoiseTexture();
    CreateFramebuffers(_width, _height);
}

SSAO::~SSAO()
{
    DestroyFramebuffers();
    DestroyNoiseTexture();
    DestroyScreenQuad();

    _normalShader.DeleteShader();
    _ssaoShader.DeleteShader();
    _compositeShader.DeleteShader();
}

void SSAO::Resize(
    int width,
    int height
)
{
    if (width <= 0 || height <= 0)
        return;

    if (
        width == _width &&
        height == _height
        )
    {
        return;
    }

    _width = width;
    _height = height;

    CreateFramebuffers(
        _width,
        _height
    );
}

void SSAO::CreateFramebuffers(
    int width,
    int height
)
{
    DestroyFramebuffers();

    // ============================================================
    // SCENE FRAMEBUFFER
    // ============================================================

    glGenFramebuffers(
        1,
        &_sceneFBO
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        _sceneFBO
    );

    // ------------------------------------------------------------
    // Scene color
    // ------------------------------------------------------------

    glGenTextures(
        1,
        &_sceneColor
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _sceneColor
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        _sceneColor,
        0
    );

    // ------------------------------------------------------------
    // Scene depth
    //
    // IMPORTANT:
    // Use 24-bit depth to match the typical GLFW default
    // framebuffer depth buffer.
    // ------------------------------------------------------------

    glGenTextures(
        1,
        &_sceneDepth
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _sceneDepth
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT24,
        width,
        height,
        0,
        GL_DEPTH_COMPONENT,
        GL_UNSIGNED_INT,
        nullptr
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        _sceneDepth,
        0
    );

    // ------------------------------------------------------------
    // Scene framebuffer draw configuration
    // ------------------------------------------------------------

    const GLenum sceneDrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0
    };

    glDrawBuffers(
        1,
        sceneDrawBuffers
    );

    CheckFramebuffer(
        _sceneFBO,
        "SSAO scene framebuffer"
    );

    // ============================================================
    // NORMAL FRAMEBUFFER
    // ============================================================

    glGenFramebuffers(
        1,
        &_normalFBO
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        _normalFBO
    );

    // ------------------------------------------------------------
    // Normal color
    // ------------------------------------------------------------

    glGenTextures(
        1,
        &_normalColor
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _normalColor
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA16F,
        width,
        height,
        0,
        GL_RGBA,
        GL_FLOAT,
        nullptr
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        _normalColor,
        0
    );

    // ------------------------------------------------------------
    // Reuse captured scene depth
    // ------------------------------------------------------------

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        _sceneDepth,
        0
    );

    const GLenum normalDrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0
    };

    glDrawBuffers(
        1,
        normalDrawBuffers
    );

    CheckFramebuffer(
        _normalFBO,
        "SSAO normal framebuffer"
    );

    // ============================================================
    // SSAO FRAMEBUFFER
    // ============================================================

    glGenFramebuffers(
        1,
        &_ssaoFBO
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        _ssaoFBO
    );

    // ------------------------------------------------------------
    // SSAO color
    // ------------------------------------------------------------

    glGenTextures(
        1,
        &_ssaoColor
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _ssaoColor
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        width,
        height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        _ssaoColor,
        0
    );

    const GLenum ssaoDrawBuffers[] =
    {
        GL_COLOR_ATTACHMENT0
    };

    glDrawBuffers(
        1,
        ssaoDrawBuffers
    );

    CheckFramebuffer(
        _ssaoFBO,
        "SSAO framebuffer"
    );

    glBindTexture(
        GL_TEXTURE_2D,
        0
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

void SSAO::DestroyFramebuffers()
{
    if (_ssaoColor != 0)
    {
        glDeleteTextures(
            1,
            &_ssaoColor
        );

        _ssaoColor = 0;
    }

    if (_ssaoFBO != 0)
    {
        glDeleteFramebuffers(
            1,
            &_ssaoFBO
        );

        _ssaoFBO = 0;
    }

    if (_normalColor != 0)
    {
        glDeleteTextures(
            1,
            &_normalColor
        );

        _normalColor = 0;
    }

    if (_normalFBO != 0)
    {
        glDeleteFramebuffers(
            1,
            &_normalFBO
        );

        _normalFBO = 0;
    }

    if (_sceneColor != 0)
    {
        glDeleteTextures(
            1,
            &_sceneColor
        );

        _sceneColor = 0;
    }

    if (_sceneDepth != 0)
    {
        glDeleteTextures(
            1,
            &_sceneDepth
        );

        _sceneDepth = 0;
    }

    if (_sceneFBO != 0)
    {
        glDeleteFramebuffers(
            1,
            &_sceneFBO
        );

        _sceneFBO = 0;
    }
}

void SSAO::CheckFramebuffer(
    GLuint framebuffer,
    const char* name
) const
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        framebuffer
    );

    const GLenum status =
        glCheckFramebufferStatus(
            GL_FRAMEBUFFER
        );

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error(
            std::string(name) +
            " is incomplete. OpenGL status: " +
            std::to_string(status)
        );
    }
}

void SSAO::CreateScreenQuad()
{
    constexpr float vertices[] =
    {
        // position    // uv
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(
        1,
        &_screenVAO
    );

    glGenBuffers(
        1,
        &_screenVBO
    );

    glBindVertexArray(
        _screenVAO
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        _screenVBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        nullptr
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        reinterpret_cast<void*>(
            2 * sizeof(float)
            )
    );

    glEnableVertexAttribArray(1);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    glBindVertexArray(0);
}

void SSAO::DestroyScreenQuad()
{
    if (_screenVBO != 0)
    {
        glDeleteBuffers(
            1,
            &_screenVBO
        );

        _screenVBO = 0;
    }

    if (_screenVAO != 0)
    {
        glDeleteVertexArrays(
            1,
            &_screenVAO
        );

        _screenVAO = 0;
    }
}

void SSAO::CreateNoiseTexture()
{
    std::mt19937 generator(7331u);
    std::uniform_real_distribution<float> random01(0.0f, 1.0f);

    std::array<
        glm::vec3,
        NOISE_SIZE* NOISE_SIZE
    > noise{};

    for (glm::vec3& value : noise)
    {
        value = glm::vec3(
            random01(generator) * 2.0f - 1.0f,
            random01(generator) * 2.0f - 1.0f,
            0.0f
        );
    }

    glGenTextures(
        1,
        &_noiseTexture
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _noiseTexture
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_REPEAT
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        NOISE_SIZE,
        NOISE_SIZE,
        0,
        GL_RGB,
        GL_FLOAT,
        noise.data()
    );

    glBindTexture(
        GL_TEXTURE_2D,
        0
    );
}

void SSAO::DestroyNoiseTexture()
{
    if (_noiseTexture != 0)
    {
        glDeleteTextures(
            1,
            &_noiseTexture
        );

        _noiseTexture = 0;
    }
}

void SSAO::CaptureScene()
{
    // ============================================================
    // Verify source depth
    // ============================================================

    //float sourceDepth = 0.0f;

    //glBindFramebuffer(
    //    GL_FRAMEBUFFER,
    //    0
    //);

    //glReadPixels(
    //    _width / 2,
    //    _height / 2,
    //    1,
    //    1,
    //    GL_DEPTH_COMPONENT,
    //    GL_FLOAT,
    //    &sourceDepth
    //);

    //std::cout
    //    << "Default framebuffer center depth: "
    //    << sourceDepth
    //    << '\n';

    // ============================================================
    // Copy default framebuffer -> scene framebuffer
    // ============================================================

    glBindFramebuffer(
        GL_READ_FRAMEBUFFER,
        0
    );

    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER,
        _sceneFBO
    );

    glReadBuffer(
        GL_BACK
    );

    glDrawBuffer(
        GL_COLOR_ATTACHMENT0
    );

    glBlitFramebuffer(
        0,
        0,
        _width,
        _height,

        0,
        0,
        _width,
        _height,

        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT,

        GL_NEAREST
    );

    // Check immediately.
    const GLenum blitError = glGetError();

    if (blitError != GL_NO_ERROR)
    {
        std::cerr
            << "SSAO depth/color blit OpenGL error: 0x"
            << std::hex
            << blitError
            << std::dec
            << '\n';
    }

    // ============================================================
    // Read back captured depth
    // ============================================================

    //float capturedDepth = 0.0f;

    //glBindFramebuffer(
    //    GL_FRAMEBUFFER,
    //    _sceneFBO
    //);

    //glReadPixels(
    //    _width / 2,
    //    _height / 2,
    //    1,
    //    1,
    //    GL_DEPTH_COMPONENT,
    //    GL_FLOAT,
    //    &capturedDepth
    //);

    //std::cout
    //    << "Captured scene depth: "
    //    << capturedDepth
    //    << '\n';

    // ============================================================
    // Restore default framebuffer
    // ============================================================

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

void SSAO::BeginNormalPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _normalFBO);

    glViewport(
        0,
        0,
        _width,
        _height
    );

    glEnable(GL_DEPTH_TEST);

    // The depth texture already contains the depth from the scene.
    // Only fragments at that visible depth should render.
    glDepthFunc(GL_LEQUAL);

    // Do not overwrite the captured scene depth.
    glDepthMask(GL_FALSE);

    glDisable(GL_BLEND);

    glClear(GL_COLOR_BUFFER_BIT);
}

void SSAO::EndNormalPass()
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    glDepthMask(GL_TRUE);
}

void SSAO::Generate(
    const glm::mat4& projection
)
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        _ssaoFBO
    );

    glDrawBuffer(
        GL_COLOR_ATTACHMENT0
    );

    glViewport(
        0,
        0,
        _width,
        _height
    );

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    glClear(
        GL_COLOR_BUFFER_BIT
    );

    _ssaoShader.Activate();

    _ssaoShader.SetMatrix(
        projection,
        "projection"
    );

    const glm::mat4 inverseProjection =
        glm::inverse(projection);

    _ssaoShader.SetMatrix(
        inverseProjection,
        "inverseProjection"
    );

    _ssaoShader.SetFloat(
        DEFAULT_RADIUS,
        "radius"
    );

    _ssaoShader.SetFloat(
        DEFAULT_BIAS,
        "bias"
    );

    _ssaoShader.SetVec3(
        glm::vec3(
            1.0f / static_cast<float>(_width),
            1.0f / static_cast<float>(_height),
            0.0f
        ),
        "texelSize"
    );

    _ssaoShader.SetFloat(
        static_cast<float>(_width) /
        static_cast<float>(NOISE_SIZE),
        "noiseScaleX"
    );

    _ssaoShader.SetFloat(
        static_cast<float>(_height) /
        static_cast<float>(NOISE_SIZE),
        "noiseScaleY"
    );

    glUniform3fv(
        _ssaoShader.GetUniform("samples"),
        KERNEL_SIZE,
        glm::value_ptr(_kernel[0])
    );

    // ------------------------------------------------------------
    // Scene depth
    // ------------------------------------------------------------

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(
        GL_TEXTURE_2D,
        _sceneDepth
    );

    _ssaoShader.SetInt(
        0,
        "sceneDepth"
    );

    // ------------------------------------------------------------
    // Scene normals
    // ------------------------------------------------------------

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(
        GL_TEXTURE_2D,
        _normalColor
    );

    _ssaoShader.SetInt(
        1,
        "sceneNormal"
    );

    // ------------------------------------------------------------
    // Noise
    // ------------------------------------------------------------

    glActiveTexture(GL_TEXTURE2);

    glBindTexture(
        GL_TEXTURE_2D,
        _noiseTexture
    );

    _ssaoShader.SetInt(
        2,
        "texNoise"
    );

    // ------------------------------------------------------------
    // Fullscreen quad
    // ------------------------------------------------------------

    glBindVertexArray(
        _screenVAO
    );

    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );

    glBindVertexArray(0);

    // ------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );
}

void SSAO::Composite()
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    glDrawBuffer(
        GL_BACK
    );

    glViewport(
        0,
        0,
        _width,
        _height
    );

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    _compositeShader.Activate();

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(
        GL_TEXTURE_2D,
        _sceneColor
    );

    _compositeShader.SetInt(
        0,
        "sceneColor"
    );

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(
        GL_TEXTURE_2D,
        _ssaoColor
    );

    _compositeShader.SetInt(
        1,
        "ssao"
    );

    glBindVertexArray(
        _screenVAO
    );

    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}