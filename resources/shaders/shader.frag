#version 460

uniform vec3 inColor;

out vec4 outputColour;

void main()
{
	outputColour = vec4(inColor, 1.0f);
}