#version 430 core

layout (location = 0) out vec4 FragNormal;

in vec3 outViewNormal;

void main()
{
    FragNormal = vec4(
        normalize(outViewNormal),
        1.0
    );
}