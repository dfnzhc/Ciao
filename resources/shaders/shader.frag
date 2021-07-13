#version 460

out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D tex1;

void main()
{
	FragColor = vec4(texture(tex1, TexCoord).xyz, 1.0);
}