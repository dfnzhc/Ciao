#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;

uniform mat4 viewModelMatrix;
uniform mat4 projMatrix;

void main()
{
	gl_Position = projMatrix * viewModelMatrix * vec4(aPos, 1.0f);
	TexCoord = aTexCoord;
}