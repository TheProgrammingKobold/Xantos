#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 outViewNormal;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 viewMatrix;

void main()
{
    mat4 modelView = viewMatrix * model;

    mat3 normalMatrix = transpose(
        inverse(mat3(modelView))
    );

    outViewNormal = normalize(
        normalMatrix * aNormal
    );

    gl_Position =
        camMatrix *
        model *
        vec4(aPos, 1.0);
}
