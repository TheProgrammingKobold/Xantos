#version 430 core
out vec4 FragColor;

in vec3 outNormal;
in vec3 outColor;
in vec2 outTexCoord;

void main()
{
   FragColor = vec4(outColor, 1.0f);
}