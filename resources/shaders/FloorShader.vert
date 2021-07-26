#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 vNormal;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

void main()
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0f);
	TexCoord = aTexCoord;
}