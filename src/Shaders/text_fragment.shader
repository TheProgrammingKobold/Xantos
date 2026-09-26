#version 430 core

out vec4 color;

in VS_OUT
{
    vec2 TexCoords;
    flat int index;
} fs_in;

uniform sampler2DArray text;
uniform vec3 textColor;

void main()
{
    float alpha = texture(
        text,
        vec3(
            fs_in.TexCoords,
            fs_in.index
        )
    ).r;

    color = vec4(textColor, alpha);
}