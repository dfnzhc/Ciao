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
    vec2 uv;
    vec3 worldPos;
};

layout (location = 0) in vertex In[];

layout (location=0) out vec2 uv;

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
    vec3 pos = interpolate3(In[0].worldPos, In[1].worldPos, In[2].worldPos);
    
    gl_Position = projMatrix * viewMatrix * vec4(pos, 1.0);
    uv = interpolate2(In[0].uv, In[1].uv, In[2].uv);
}