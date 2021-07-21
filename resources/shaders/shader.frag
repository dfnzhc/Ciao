#version 460

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vNormal;

// texture samplers
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_emissive1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_lightMap1;
uniform sampler2D texture_unknown1;

void main()
{
	//FragColor = vec4(vNormal.xyz, 1.0);
	FragColor = vec4(texture(texture_unknown1, TexCoord).xyz, 1.0);
}