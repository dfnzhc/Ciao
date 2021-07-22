#version 460

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vNormal;

// texture samplers
uniform sampler2D Tex_Diffuse1;
uniform sampler2D Tex_Emissive1;
uniform sampler2D Tex_Normal1;
uniform sampler2D Tex_LightMap1;
uniform sampler2D Tex_Unknown1;

void main()
{
	//FragColor = vec4(vNormal.xyz, 1.0);
	FragColor = vec4(texture(Tex_Normal1, TexCoord).xyz, 1.0);
}