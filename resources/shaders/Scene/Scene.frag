#version 460 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : enable

#include "MaterialData.glsl"

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
};

layout(std430, binding = 2) restrict readonly buffer Materials
{
    MaterialData in_Materials[];
};

layout (location=0) in vec2 v_tc;
layout (location=1) in vec3 v_worldNormal;
layout (location=2) in vec3 v_worldPos;
layout (location=3) in flat uint matIdx;

layout (location=0) out vec4 out_FragColor;

#include "../Common.glsl"

void main()
{
    MaterialData mtl = in_Materials[matIdx];

    vec4 albedo = mtl.albedoColor_;
    vec3 normalSample = vec3(0.0, 0.0, 0.0);

    if (mtl.albedoMap_ > 0) 
        albedo = SRGBtoLINEAR(texture( sampler2D(unpackUint2x32(mtl.albedoMap_)), v_tc));
    if (mtl.normalMap_ > 0)
        normalSample = texture( sampler2D(unpackUint2x32(mtl.normalMap_)), v_tc).xyz;

    runAlphaTest(albedo.a, mtl.alphaTest_);

    // world-space normal
    vec3 n = normalize(v_worldNormal);

    // normal mapping: skip missing normal maps
    if (length(normalSample) > 0.5)
        n = perturbNormal(normalSample, n, normalize(cameraPos.xyz - v_worldPos.xyz), v_tc);

    vec3 lightDir = normalize(vec3(-1.0, 1.0, 0.1));

    float NdotL = clamp( dot( n, lightDir ), 0.3, 1.0 );
    
    albedo.rgb = pow(albedo.rgb * NdotL, vec3(1.0 / 2.2));

    
    out_FragColor = vec4(vec3(albedo), 1.0);
}
