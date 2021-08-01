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
	vec4 cameraPos;
};

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

// OpenGL's Z is in -1..1
const mat4 scaleBias = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0 );

void main()
{
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0f);
	tc = aTexCoord;
	
	normal = normalize(normalMatrix * aNormal);
	worldPos = (modelMatrix * vec4(aPos, 1.0f)).xyz;

	FragPosLightSpace = scaleBias * lightSpaceMatrix * vec4(worldPos, 1.0);
}