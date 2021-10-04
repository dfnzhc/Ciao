#version 460

layout (location=0) out vec3 dir;

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 view;
	mat4 proj;
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

	vec4 npos = proj * view * vec4(500.0 * pos[idx], 1.0);
	//gl_Position = npos.xyzw;
	gl_Position = npos.xyww; // 可以通过深度缓冲优化，最后绘制天空盒
}