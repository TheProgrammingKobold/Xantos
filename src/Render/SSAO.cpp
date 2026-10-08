#include "SSAO.h"

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cmath>
#include <random>
#include <stdexcept>
#include <string>

SSAO::SSAO(
    int width,
    int height
)
    : _ssaoShader(
        "ssao_screen_vertex.shader",
        "ssao_fragment.shader"
    ),
    _compositeShader(
        "ssao_screen_vertex.shader",
        "ssao_composite_fragment.shader"
    )
{
    _width = width > 0 ? width : 1;
    _height = height > 0 ? height : 1;

    std::mt19937 generator(0x51A9B8u);
    std::uniform_real_distribution<float> random01(0.0f, 1.0f);
    std::uniform_real_distribution<float> randomSigned(-1.0f, 1.0f);

    for (int i = 0; i < KERNEL_SIZE; ++i)
    {
        glm::vec3 sample;

        do
        {
            sample = glm::vec3(
                randomSigned(generator),
                randomSigned(generator),
                random01(generator)
            );
        }
        while (
            glm::dot(sample, sample) > 1.0f ||
            glm::dot(sample, sample) < 0.0001f
        );

        sample = glm::normalize(sample);

        const float randomLength = random01(generator);
        const float t =
            static_cast<float>(i) /
            static_cast<float>(KERNEL_SIZE - 1);

        const float scale =
            0.1f +
            (t * t) * 0.9f;

        _kernel[i] = sample * randomLength * scale;
    }

    CreateScreenQuad();
    CreateNoiseTexture();
    CreateFramebuffers(_width, _height);

    _ssaoShader.Activate();

    glUniform3fv(
        glGetUniformLocation(
            _ssaoShader.GetID(),
            "samples"
        ),
        KERNEL_SIZE,
        glm::value_ptr(_kernel[0])
    );

    _ssaoShader.SetInt(0, "sceneDepth");
    _ssaoShader.SetInt(1, "texNoise");
    _ssaoShader.SetFloat(DEFAULT_RADIUS, "radius");
    _ssaoShader.SetFloat(DEFAULT_BIAS, "bias");

    _compositeShader.Activate();
    _compositeShader.SetInt(0, "sceneColor");
    _compositeShader.SetInt(1, "ssao");

    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
}

SSAO::~SSAO()
{
    DestroyFramebuffers();
    DestroyNoiseTexture();
    DestroyScreenQuad();

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

    if (_width == width && _height == height)
        return;

    _width = width;
    _height = height;

    CreateFramebuffers(
        _width,
        _height
    );
}

void SSAO::CaptureScene()
{
    // Copy the scene exactly as it was rendered by the original renderer.
    // The terrain therefore never has to render into our custom FBO.

    glBindFramebuffer(
        GL_READ_FRAMEBUFFER,
        0
    );

    glBindFramebuffer(
        GL_DRAW_FRAMEBUFFER,
        _sceneFBO
    );

    glReadBuffer(GL_BACK);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glBlitFramebuffer(
        0,
        0,
        _width,
        _height,
        0,
        0,
        _width,
        _height,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    glViewport(
        0,
        0,
        _width,
        _height
    );

    glActiveTexture(GL_TEXTURE0);
}

void SSAO::Generate(
    const glm::mat4& projection
)
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        _ssaoFBO
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

    glClear(GL_COLOR_BUFFER_BIT);

    _ssaoShader.Activate();

    _ssaoShader.SetMatrix(
        projection,
        "projection"
    );

    _ssaoShader.SetMatrix(
        glm::inverse(projection),
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(
        GL_TEXTURE_2D,
        _sceneDepth
    );

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(
        GL_TEXTURE_2D,
        _noiseTexture
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

    glBindVertexArray(_screenVAO);

    glDrawArrays(
        GL_TRIANGLE_STRIP,
        0,
        4
    );

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    glViewport(
        0,
        0,
        _width,
        _height
    );

    glActiveTexture(GL_TEXTURE0);
}

void SSAO::Composite()
{
    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
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

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(
        GL_TEXTURE_2D,
        _ssaoColor
    );

    glBindVertexArray(_screenVAO);

    glDrawArrays(
        GL_TRIANGLE_STRIP,
        0,
        4
    );

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

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
}

void SSAO::CreateScreenQuad()
{
    constexpr float vertices[] =
    {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &_screenVAO);
    glGenBuffers(1, &_screenVBO);

    glBindVertexArray(_screenVAO);

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
        reinterpret_cast<void*>(2 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SSAO::DestroyScreenQuad()
{
    if (_screenVBO != 0)
    {
        glDeleteBuffers(1, &_screenVBO);
        _screenVBO = 0;
    }

    if (_screenVAO != 0)
    {
        glDeleteVertexArrays(1, &_screenVAO);
        _screenVAO = 0;
    }
}

void SSAO::CreateNoiseTexture()
{
    std::array<glm::vec3, NOISE_SIZE * NOISE_SIZE> noise{};

    std::mt19937 generator(0xBADC0DEu);
    std::uniform_real_distribution<float> randomSigned(-1.0f, 1.0f);

    for (glm::vec3& value : noise)
    {
        value = glm::normalize(
            glm::vec3(
                randomSigned(generator),
                randomSigned(generator),
                0.0f
            )
        );
    }

    glGenTextures(1, &_noiseTexture);

    glBindTexture(
        GL_TEXTURE_2D,
        _noiseTexture
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

void SSAO::CreateFramebuffers(
    int width,
    int height
)
{
    DestroyFramebuffers();

    // Scene color + depth.
    glGenFramebuffers(
        1,
        &_sceneFBO
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        _sceneFBO
    );

    glGenTextures(
        1,
        &_sceneColor
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _sceneColor
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

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        _sceneColor,
        0
    );

    glGenTextures(
        1,
        &_sceneDepth
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _sceneDepth
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT24,
        width,
        height,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
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

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_COMPARE_MODE,
        GL_NONE
    );

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        _sceneDepth,
        0
    );

    const GLenum sceneDrawBuffer =
        GL_COLOR_ATTACHMENT0;

    glDrawBuffers(
        1,
        &sceneDrawBuffer
    );

    CheckFramebuffer(
        _sceneFBO,
        "Scene framebuffer"
    );

    // SSAO output.
    glGenFramebuffers(
        1,
        &_ssaoFBO
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        _ssaoFBO
    );

    glGenTextures(
        1,
        &_ssaoColor
    );

    glBindTexture(
        GL_TEXTURE_2D,
        _ssaoColor
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

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        _ssaoColor,
        0
    );

    const GLenum ssaoDrawBuffer =
        GL_COLOR_ATTACHMENT0;

    glDrawBuffers(
        1,
        &ssaoDrawBuffer
    );

    CheckFramebuffer(
        _ssaoFBO,
        "SSAO framebuffer"
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    glBindTexture(
        GL_TEXTURE_2D,
        0
    );

    glActiveTexture(GL_TEXTURE0);
}

void SSAO::DestroyFramebuffers()
{
    if (_sceneDepth != 0)
    {
        glDeleteTextures(
            1,
            &_sceneDepth
        );

        _sceneDepth = 0;
    }

    if (_sceneColor != 0)
    {
        glDeleteTextures(
            1,
            &_sceneColor
        );

        _sceneColor = 0;
    }

    if (_sceneFBO != 0)
    {
        glDeleteFramebuffers(
            1,
            &_sceneFBO
        );

        _sceneFBO = 0;
    }

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
        glCheckFramebufferStatus(GL_FRAMEBUFFER);

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error(
            std::string(name) +
            " is incomplete. OpenGL status code: " +
            std::to_string(
                static_cast<unsigned int>(status)
            )
        );
    }
}
