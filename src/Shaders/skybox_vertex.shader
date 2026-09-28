#version 430 core

layout (location = 0) in vec3 position;

out vec3 TexCoord;

uniform mat4 camMatrix;

void main()
{
    TexCoord = position;

    vec4 pos = camMatrix * vec4(position, 1.0);

    gl_Position = pos.xyww;
}