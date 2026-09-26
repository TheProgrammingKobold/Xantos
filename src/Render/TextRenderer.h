#pragma once

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "VAO.h"
#include "VBO.h"

class TextRenderer
{
public:

    TextRenderer(
        const std::string& fontPath,
        unsigned int fontSize = 256,
        unsigned int maxCharacters = 400
    );

    ~TextRenderer();

    void RenderText(
        const std::string& text,
        float x,
        float y,
        float scale,
        const glm::vec3& color
    );

    void RenderTextRelative(
        const std::string& text,
        float xFraction,
        float yFraction,
        float heightFraction,
        const glm::vec3& color
    );

    void SetProjection(const glm::mat4& projection);
    void SetViewportSize(int width, int height);

private:

    struct Character
    {
        int textureID;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };

    void LoadFont(
        const std::string& fontPath,
        unsigned int fontSize
    );

    void CreateBuffers();

    void TextRenderCall(int length);

private:

    std::map<GLchar, Character> _Characters;

    GLuint _textureArray = 0;
    glm::ivec2 _atlasSize = glm::ivec2(0);
    glm::ivec2 _viewportSize = glm::ivec2(0);

    VAO _VAO;
    VBO _VBO;

    GLuint _transformVBO = 0;
    GLuint _letterVBO = 0;
    GLuint _glyphUVScaleVBO = 0;

    std::vector<glm::mat4> _transforms;
    std::vector<int> _letterMap;
    std::vector<glm::vec2> _glyphUVScales;

    unsigned int _arrayLimit;

    Shader _shader;
};