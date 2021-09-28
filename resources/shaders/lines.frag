#version 460

layout (location=0) out vec4 FragColor;

struct PerVertex
{
	vec4 color;
};

layout (location=0) in PerVertex vtx;

void main()
{
	FragColor = vtx.color;
}