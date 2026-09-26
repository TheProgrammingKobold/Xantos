#version 430 core

layout (location = 0) in vec2 vertex;

layout (location = 1) in mat4 transform;
layout (location = 5) in int letter;
layout (location = 6) in vec2 glyphUVScale;

out VS_OUT
{
    vec2 TexCoords;
    flat int index;
} vs_out;

uniform mat4 projection;

void main()
{
    gl_Position =
        projection *
        transform *
        vec4(vertex, 0.0, 1.0);

    vs_out.index = letter;

    vs_out.TexCoords = vec2(vertex.x, 1.0 - vertex.y) * glyphUVScale;
}