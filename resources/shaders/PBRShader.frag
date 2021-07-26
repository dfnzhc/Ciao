#version 460

layout (location=0) out vec4 FragColor;

uniform vec3 cameraPos;

layout (location=0) in vec2 tc;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 worldPos;

// texture samplers
layout (binding = 0) uniform sampler2D texAO;
layout (binding = 1) uniform sampler2D texEmissive;
layout (binding = 2) uniform sampler2D texAlbedo;
layout (binding = 3) uniform sampler2D texMetalRoughness;
layout (binding = 4) uniform sampler2D texNormal;

layout (binding = 5) uniform samplerCube texEnvMap;
layout (binding = 6) uniform samplerCube texEnvMapIrradiance;
layout (binding = 7) uniform sampler2D texBRDF_LUT;

#include "PBR.sp"

void main()
{
	vec4 Kao = texture(texAO, tc);
	vec4 Ke = texture(texEmissive, tc);
	vec4 Kd = texture(texAlbedo, tc);
	vec2 MeR = texture(texMetalRoughness, tc).yz;

	// world-space normal
	vec3 n = normalize(normal);
	vec3 normalSample = texture(texNormal, tc).xyz;

	// normal mapping
	n = perturbNormal(n, normalize(cameraPos.xyz - worldPos), normalSample, tc);
	vec4 mrSample = texture(texMetalRoughness, tc);

	PBRInfo pbrInputs;
	Ke.rgb = SRGBtoLINEAR(Ke).rgb;
	
	// image-based lighting
	vec3 color = calculatePBRInputsMetallicRoughness(Kd, n, cameraPos.xyz, worldPos, mrSample, pbrInputs);
	// one hardcoded light source
	color += calculatePBRLightContribution( pbrInputs, normalize(vec3(-1.0, -1.0, -1.0)), vec3(1.0) );
	color += calculatePBRLightContribution( pbrInputs, normalize(vec3(-1.0, -1.0, -1.0)), vec3(1.0) );
	// ambient occlusion
	color = color * ( Kao.r < 0.01 ? 1.0 : Kao.r );
	// emissive
	color = pow( Ke.rgb + color, vec3(1.0/2.2) );
	
	
	FragColor = vec4(color, 1.0);
}

