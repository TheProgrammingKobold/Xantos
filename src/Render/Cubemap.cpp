#include "Cubemap.h"

#include <filesystem>
#include <memory>
#include <stdexcept>

#include <stb_image.h>

#include "../Util/File.h"

Cubemap::Cubemap(const std::array<std::string, 6>& faces)
{
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        int width;
        int height;
        int channels;

        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(
            faces[i].c_str(),
            &width,
            &height,
            &channels,
            0
        );

        if (!data)
        {
            glDeleteTextures(1, &_id);
            _id = 0;

            throw std::runtime_error(
                "Failed to load cubemap face: " + faces[i]
            );
        }

        GLenum format;

        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;
        else
        {
            stbi_image_free(data);

            glDeleteTextures(1, &_id);
            _id = 0;

            throw std::runtime_error(
                "Unsupported image format: " + faces[i]
            );
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        stbi_image_free(data);
    }

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_R,
        GL_CLAMP_TO_EDGE
    );

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &_id);
}

void Cubemap::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
}
