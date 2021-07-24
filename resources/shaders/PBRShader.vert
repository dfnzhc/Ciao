#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 vNormal;
out vec3 worldPos;

uniform mat4 modelViewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;

void main()
{
	gl_Position = projMatrix * modelViewMatrix * vec4(aPos, 1.0f);
	worldPos = (modelMatrix * vec4(aPos, 1.0f)).xyz;
	
	TexCoord = aTexCoord;
	vNormal = normalize(normalMatrix * aNormal);
}