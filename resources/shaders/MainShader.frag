#version 460

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vNormal;

// texture samplers
uniform sampler2D Texture1;

void main()
{
	//FragColor = vec4(vNormal.xyz, 1.0);
	vec3 albedo = pow(texture(Texture1, TexCoord).rgb, 2.2);
	FragColor = vec4(albedo, 1.0);
}