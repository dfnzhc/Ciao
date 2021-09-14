#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
    float tesselationScale;
};

const vec3 pos[4] = vec3[4](
    vec3(-1.0, 0.0, -1.0),
    vec3( 1.0, 0.0, -1.0),
    vec3( 1.0, 0.0,  1.0),
    vec3(-1.0, 0.0,  1.0)
);

const vec2 tc[4] = vec2[4](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),
    vec2(0.0, 1.0)
);

const int indices[6] = int[6](
    0, 1, 2, 2, 3, 0
);

const vec3 n = vec3(0, 1, 0);

layout (location = 0) out vec3 worldPos;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec3 normal;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main()
{
    int idx = indices[gl_VertexID];
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(pos[idx], 1.0f);
    worldPos = (modelMatrix * vec4(pos[idx], 1.0f)).xyz;

    uv = tc[idx];
    normal = normalMatrix * n;
}