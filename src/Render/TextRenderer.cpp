#include "TextRenderer.h"

// Ill be honest this entire thing was made with AI.

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


TextRenderer::TextRenderer(
    const std::string& fontPath,
    unsigned int fontSize,
    unsigned int maxCharacters
)
    : _VBO(
        std::vector<float>
{
        // x    y
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
}
),
_arrayLimit(maxCharacters),
_shader("text_vertex.shader", "text_fragment.shader")
{
    _transforms.resize(_arrayLimit, glm::mat4(1.0f));
    _letterMap.resize(_arrayLimit, 0);
    _glyphUVScales.resize(_arrayLimit, glm::vec2(0.0f));

    LoadFont(fontPath, fontSize);
    CreateBuffers();
}


TextRenderer::~TextRenderer()
{
    if (_textureArray != 0)
        glDeleteTextures(1, &_textureArray);

    if (_transformVBO != 0)
        glDeleteBuffers(1, &_transformVBO);

    if (_letterVBO != 0)
        glDeleteBuffers(1, &_letterVBO);

    if (_glyphUVScaleVBO != 0)
        glDeleteBuffers(1, &_glyphUVScaleVBO);

    _VAO.deleteObject();
    _VBO.deleteObject();
}


void TextRenderer::LoadFont(
    const std::string& fontPath,
    unsigned int fontSize
)
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library"
            << std::endl;
        return;
    }

    FT_Face face;

    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font: "
            << fontPath << std::endl;

        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    _lineHeight = static_cast<float>(face->size->metrics.height) / 64.0f;


    // --------------------------------------------------
    // Find the largest glyph dimensions.
    // --------------------------------------------------

    unsigned int maxWidth = 0;
    unsigned int maxHeight = 0;

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph "
                << static_cast<int>(c) << std::endl;
            continue;
        }

        maxWidth = std::max(
            maxWidth,
            static_cast<unsigned int>(face->glyph->bitmap.width)
        );

        maxHeight = std::max(
            maxHeight,
            static_cast<unsigned int>(face->glyph->bitmap.rows)
        );
    }

    _atlasSize = glm::ivec2(maxWidth, maxHeight);

    // --------------------------------------------------
    // Create texture array.
    // --------------------------------------------------

    glGenTextures(1, &_textureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, _textureArray);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RED,
        maxWidth,
        maxHeight,
        128,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        nullptr
    );


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    // --------------------------------------------------
    // Load ASCII glyphs into texture array.
    // --------------------------------------------------

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph "
                << static_cast<int>(c) << std::endl;
            continue;
        }

        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0,
            0,
            c,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            1,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );


        Character character;

        character.textureID = c;

        character.size = glm::ivec2(
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows
        );

        character.bearing = glm::ivec2(
            face->glyph->bitmap_left,
            face->glyph->bitmap_top
        );

        character.advance = face->glyph->advance.x;


        _Characters.insert(
            std::pair<char, Character>(c, character)
        );
    }


    glTexParameteri(
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );


    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);


    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


void TextRenderer::CreateBuffers()
{
    // --------------------------------------------------
    // Quad VAO/VBO
    // --------------------------------------------------

    _VAO.bind();

    _VBO.bind();

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    _VBO.unbind();


    // --------------------------------------------------
    // Transform buffer
    // --------------------------------------------------

    glGenBuffers(1, &_transformVBO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        _transformVBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        _transforms.size() * sizeof(glm::mat4),
        _transforms.data(),
        GL_DYNAMIC_DRAW
    );


    // A mat4 takes four attribute locations.

    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(1 + i);

        glVertexAttribPointer(
            1 + i,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(glm::mat4),
            (void*)(sizeof(glm::vec4) * i)
        );

        glVertexAttribDivisor(1 + i, 1);
    }


    // --------------------------------------------------
    // Letter buffer
    // --------------------------------------------------

    glGenBuffers(1, &_letterVBO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        _letterVBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        _letterMap.size() * sizeof(int),
        _letterMap.data(),
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(5);

    glVertexAttribIPointer(
        5,
        1,
        GL_INT,
        sizeof(int),
        nullptr
    );

    glVertexAttribDivisor(5, 1);

    glGenBuffers(1, &_glyphUVScaleVBO);

    glBindBuffer(GL_ARRAY_BUFFER, _glyphUVScaleVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        _glyphUVScales.size() * sizeof(glm::vec2),
        _glyphUVScales.data(),
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(6);

    glVertexAttribPointer(
        6,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec2),
        nullptr
    );

    glVertexAttribDivisor(6, 1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _VAO.unbind();
}


void TextRenderer::SetProjection(
    const glm::mat4& projection
)
{
    _shader.activate();

    _shader.setMatrix(
        projection,
        "projection"
    );
}


void TextRenderer::SetViewportSize(int width, int height)
{
    _viewportSize = glm::ivec2(width, height);
}


void TextRenderer::RenderTextRelative(
    const std::string& text,
    float xFraction,
    float yFraction,
    float heightFraction,
    const glm::vec3& color
)
{
    if (
        text.empty() ||
        _viewportSize.x <= 0 ||
        _viewportSize.y <= 0 ||
        _atlasSize.y <= 0 ||
        heightFraction <= 0.0f
    )
    {
        return;
    }

    float textHeight = heightFraction * _viewportSize.y;
    float scale = textHeight / (_atlasSize.y * (48.0f / 256.0f));

    RenderText(
        text,
        xFraction * _viewportSize.x,
        yFraction * _viewportSize.y,
        scale,
        color
    );
}


void TextRenderer::RenderText(
    const std::string& text,
    float x,
    float y,
    float scale,
    const glm::vec3& color
)
{
    if (text.empty())
        return;

    scale = scale * 48.0f / 256.0f;

    const float startX = x;
    int length = 0;

    for (char c : text)
    {
        if (c == '\n')
        {
            x = startX;
            y += _lineHeight * scale;
            continue;
        }

        if (length >= static_cast<int>(_arrayLimit))
            break;

        auto iterator = _Characters.find(c);

        if (iterator == _Characters.end())
            continue;

        Character& ch = iterator->second;


        float xpos =
            x + ch.bearing.x * scale;

        float ypos =
            y - (ch.size.y - ch.bearing.y) * scale;

        _transforms[length] =
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(xpos, ypos, 0.0f)
            )
            *
            glm::scale(
                glm::mat4(1.0f),
                glm::vec3(
                    ch.size.x * scale,
                    ch.size.y * scale,
                    1.0f
                )
            );


        _letterMap[length] = ch.textureID;
        _glyphUVScales[length] = glm::vec2(
            static_cast<float>(ch.size.x) / _atlasSize.x,
            static_cast<float>(ch.size.y) / _atlasSize.y
        );

        x += (ch.advance >> 6) * scale;

        length++;
    }


    if (length == 0)
        return;


    // --------------------------------------------------
    // Upload instance data.
    // --------------------------------------------------

    glBindBuffer(
        GL_ARRAY_BUFFER,
        _transformVBO
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        length * sizeof(glm::mat4),
        _transforms.data()
    );


    glBindBuffer(
        GL_ARRAY_BUFFER,
        _letterVBO
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        length * sizeof(int),
        _letterMap.data()
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        _glyphUVScaleVBO
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        length * sizeof(glm::vec2),
        _glyphUVScales.data()
    );


    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // --------------------------------------------------
    // Render text.
    // --------------------------------------------------

    _shader.activate();

    _shader.setVec3(
        color,
        "textColor"
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(
        GL_TEXTURE_2D_ARRAY,
        _textureArray
    );

    _VAO.bind();

    glDrawArraysInstanced(
        GL_TRIANGLE_STRIP,
        0,
        4,
        length
    );

    _VAO.unbind();

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}