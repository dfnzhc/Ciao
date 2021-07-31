#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

layout (location=0) out vec2 tc;
layout (location=1) out vec3 normal;
layout (location=2) out vec3 worldPos;
layout (location=3) out vec4 FragPosLightSpace;

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 viewMatrix;
	mat4 projMatrix;
};

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0f);
	tc = aTexCoord;

	normal = normalize(normalMatrix * aNormal);
	worldPos = (modelMatrix * vec4(aPos, 1.0f)).xyz;

	FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
}