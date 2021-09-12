#version 460

#include "GridParameters.h"

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
};

layout (location = 0) out vec2 uv;

void main()
{
    int idx = indices[gl_VertexID];
    vec3 position = pos[idx] * gridSize;
    
    gl_Position = projMatrix * viewMatrix * vec4(position, 1.0);
    uv = position.xz;
}