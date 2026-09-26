#version 430 core
out vec4 FragColor;

in vec3 outNormal;
in vec3 outColor;
in vec2 outTexCoord;

uniform sampler2D tex;

void main()
{
   vec4 texColor = texture(tex, outTexCoord);
   //FragColor = vec4(outColor, 1.0f);
   FragColor = texColor;
}