#include "Mesh.h"

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices
)
    : _vbo(vertices),
    _ebo(indices),
    _indexCount(
        static_cast<GLsizei>(indices.size())
    )
{
    _vao.bind();

    _vbo.bind();
    _ebo.bind();

    _vao.linkAttrib(
        _vbo,
        0,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)0
    );

    _vao.linkAttrib(
        _vbo,
        1,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)(3 * sizeof(float))
    );

    _vao.linkAttrib(
        _vbo,
        2,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)(6 * sizeof(float))
    );

    _vao.linkAttrib(
        _vbo,
        3,
        2,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)(9 * sizeof(float))
    );

    _vao.unbind();
    _vbo.unbind();
    _ebo.unbind();
}

void Mesh::Bind() const
{
    _vao.bind();
}

void Mesh::Unbind() const
{
    _vao.unbind();
}

void Mesh::Draw() const
{
    _vao.bind();

    glDrawElements(
        GL_TRIANGLES,
        _indexCount,
        GL_UNSIGNED_INT,
        nullptr
    );
}

GLsizei Mesh::GetIndexCount() const
{
    return _indexCount;
}