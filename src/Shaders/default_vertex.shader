#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec3 outNormal;
out vec3 outColor;
out vec2 outTexCoord;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
   gl_Position = camMatrix * model * vec4(aPos, 1.0);
   outNormal = aNormal;
   outColor = aColor;
   outTexCoord = aTexCoord;
}