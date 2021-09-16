#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
    float tesselationScale;
};

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout (location=0) out vec4 out_FragColor;

layout (binding = 1) uniform sampler2D tex_Diffuse;
layout (binding = 2) uniform sampler2D tex_Displacement;
layout (binding = 3) uniform sampler2D tex_Normal;


void main()
{
    vec3 n = normalize(normal);
    
    vec3 normalSample = texture(tex_Normal, uv).xyz;
    // n = GetNormal(n, normalize(cameraPos.xyz - worldPos), normalSample, uv);
    out_FragColor = vec4(n, 1.0);
}