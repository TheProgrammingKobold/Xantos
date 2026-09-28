#pragma once
#include <GLAD/glad.h>
#include <array>
#include <string>

class Cubemap
{
public:
    Cubemap(const std::array<std::string, 6>& faces);
    ~Cubemap();

    Cubemap(const Cubemap&) = delete;
    Cubemap& operator=(const Cubemap&) = delete;

    void Bind(unsigned int slot = 0) const;

private:
    GLuint _id = 0;
};