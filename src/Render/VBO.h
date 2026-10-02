#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include "Vertex.h"

class VBO
{
public:

    VBO(const std::vector<Vertex>& vertices);
    VBO(const std::vector<float>& vertices);

    void Bind();
    void Unbind();
    void DeleteObject() const;

    const GLuint GetID() const { return _ID; }

private:

    GLuint _ID;
};