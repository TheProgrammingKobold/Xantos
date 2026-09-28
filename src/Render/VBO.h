#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

struct Vertex
{
	glm::vec3 position, normal, color;
	glm::vec2 texUV;
};

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