#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
};

layout (location=0) in vec3 pos;
layout (location=1) in vec2 tc;
layout (location=2) in vec3 n;

layout (location=0) out vec2 uv;
layout (location=1) out vec3 wpos;
layout (location=2) out vec3 normal;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main()
{
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);

    uv = tc;
    wpos = vec3(modelMatrix * vec4(pos, 1.0));
    
    normal = normalMatrix * n; 
}