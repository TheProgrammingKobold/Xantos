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
    _vao.Bind();

    _vbo.Bind();
    _ebo.Bind();

    _vao.LinkAttrib(
        _vbo,
        0,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)0
    );

    _vao.LinkAttrib(
        _vbo,
        1,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)(3 * sizeof(float))
    );

    _vao.LinkAttrib(
        _vbo,
        2,
        3,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)(6 * sizeof(float))
    );

    _vao.LinkAttrib(
        _vbo,
        3,
        2,
        GL_FLOAT,
        sizeof(Vertex),
        (void*)(9 * sizeof(float))
    );

    _vao.Unbind();
    _vbo.Unbind();
    _ebo.Unbind();
}

void Mesh::Bind() const
{
    _vao.Bind();
}

void Mesh::Unbind() const
{
    _vao.Unbind();
}

void Mesh::Draw() const
{
    _vao.Bind();

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