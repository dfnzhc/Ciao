#version 460

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	TexCoords = aPos;
	vec4 pos = projMatrix * viewMatrix * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}