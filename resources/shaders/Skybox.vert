#version 460

layout (location = 0) in vec3 aPos;

layout (location=0) out vec3 dir;

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPos;
};

const vec3 pos[8] = vec3[8](
	vec3(-1.0,-1.0, 1.0),
	vec3( 1.0,-1.0, 1.0),
	vec3( 1.0, 1.0, 1.0),
	vec3(-1.0, 1.0, 1.0),

	vec3(-1.0,-1.0,-1.0),
	vec3( 1.0,-1.0,-1.0),
	vec3( 1.0, 1.0,-1.0),
	vec3(-1.0, 1.0,-1.0)
);

const int indices[36] = int[36](
	// front
	0, 1, 2, 2, 3, 0,
	// right
	1, 5, 6, 6, 2, 1,
	// back
	7, 6, 5, 5, 4, 7,
	// left
	4, 0, 3, 3, 7, 4,
	// bottom
	4, 5, 1, 1, 0, 4,
	// top
	3, 2, 6, 6, 7, 3
);

void main()
{
	int idx = indices[gl_VertexID];
	dir = pos[idx].xyz;
	
	mat4 viewMat = mat4(mat3(viewMatrix));
	vec4 pos = projMatrix * viewMat * vec4(pos[idx], 1.0);
    gl_Position = pos.xyww;
}