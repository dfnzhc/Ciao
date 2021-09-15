#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
};

layout (location=0) in vec2 uv;
layout (location=1) in vec3 wpo;
layout (location=2) in vec3 normal;

layout (location=0) out vec4 out_FragColor;

layout (binding = 0) uniform sampler2D tex_Diff;    // albedo
layout (binding = 1) uniform sampler2D tex_AO;      // ambient occlusion
layout (binding = 2) uniform sampler2D tex_Em;      // emissive
layout (binding = 3) uniform sampler2D tex_Me_R;     // metal and roughness
layout (binding = 4) uniform sampler2D tex_Nor;     // normal

#include "../Common.glsl"

void main()
{
    vec4 Kd  = texture(tex_Diff, uv);
    vec4 Kao = texture(tex_AO, uv);
    vec4 Ke  = texture(tex_Em, uv);
    vec2 MeR = texture(tex_Me_R, uv).yz;
    vec3 normalSample = texture(tex_Nor, uv).xyz;
    
    // world-space normal
    vec3 n = normalize(normal);


    // normal mapping
    n = perturbNormal(normalSample, n, normalize(cameraPos.xyz - wpo), uv);
    
    out_FragColor = vec4(n, 1.0);
}