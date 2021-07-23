#version 460

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vNormal;

// texture samplers
uniform sampler2D Texture1;

void main()
{
	//FragColor = vec4(vNormal.xyz, 1.0);
	FragColor = vec4(texture(Texture1, TexCoord).xyz, 1.0);
}