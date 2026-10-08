#version 430 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D sceneColor;
uniform sampler2D ssao;

void main()
{
    vec4 scene = texture(
        sceneColor,
        TexCoord
    );

    float ao = texture(
        ssao,
        TexCoord
    ).r;

    FragColor = vec4(
        scene.rgb * ao,
        scene.a
    );
}
