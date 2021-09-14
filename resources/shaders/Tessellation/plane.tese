#version 460 core

layout(triangles, equal_spacing, ccw) in;

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
    float tesselationScale;
};

struct vertex
{
    vec3 worldPos;
    vec2 uv;
    vec3 normal;
};

layout(location = 0) in vertex In[];

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec2 uv;
layout(location = 2) out vec3 normal;

layout (binding = 2) uniform sampler2D tex_Displacement;

uniform float dispFactor;

vec2 interpolate2(in vec2 v0, in vec2 v1, in vec2 v2)
{
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

vec3 interpolate3(in vec3 v0, in vec3 v1, in vec3 v2)
{
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

vec4 interpolate4(in vec4 v0, in vec4 v1, in vec4 v2)
{
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

void main()
{
    uv = interpolate2(In[0].uv, In[1].uv, In[2].uv);
    normal = interpolate3(In[0].normal, In[1].normal, In[2].normal);
    normal = normalize(normal);
    
    worldPos = interpolate3(In[0].worldPos, In[1].worldPos, In[2].worldPos);
    
    // 顶点位置向法线偏移
    float Displacement = texture(tex_Displacement, uv).x;
    worldPos += normal * Displacement * dispFactor;
    gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1.0);
}