#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
};

struct Vertex
{
    float p[3];
    float tc[2];
};

layout(std430, binding = 0) restrict readonly buffer Vertices
{
    Vertex in_Vertices[];
};

vec3 getPosition(int i)
{
    return vec3(in_Vertices[i].p[0], in_Vertices[i].p[1], in_Vertices[i].p[2]);
}

vec2 getTexCoord(int i)
{
    return vec2(in_Vertices[i].tc[0], in_Vertices[i].tc[1]);
}

layout (location=0) out vec2 uv;

uniform mat4 modelMatrix;

void main()
{
    vec3 pos = getPosition(gl_VertexID);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);

    uv = getTexCoord(gl_VertexID);
}