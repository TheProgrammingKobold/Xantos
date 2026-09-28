#include "VBO.h"

VBO::VBO(const std::vector<Vertex>& vertices)
{
    glGenBuffers(1, &_ID);

    glBindBuffer(GL_ARRAY_BUFFER, _ID);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );
}

VBO::VBO(const std::vector<float>& vertices)
{
    glGenBuffers(1, &_ID);

    glBindBuffer(GL_ARRAY_BUFFER, _ID);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );
}

void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, _ID);
}

void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::DeleteObject() const
{
    glDeleteBuffers(1, &_ID);
}