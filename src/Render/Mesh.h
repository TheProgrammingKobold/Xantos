#pragma once

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include <vector>

#include <glad/glad.h>

class Mesh
{
public:
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<GLuint>& indices
    );

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Bind() const;
    void Unbind() const;

    void Draw() const;

    GLsizei GetIndexCount() const;

private:
    VAO _vao;
    VBO _vbo;
    EBO _ebo;

    GLsizei _indexCount;
};