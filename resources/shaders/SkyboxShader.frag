#version 460

out vec4 FragColor;

in vec3 TexCoords;

// texture samplers
uniform samplerCube  skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);
}