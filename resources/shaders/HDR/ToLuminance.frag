#version 460 core

layout (location=0) in vec2 uv;

layout (location=0) out vec4 out_FragColor;

layout(binding = 0) uniform sampler2D tex_;

void main()
{
    vec4 color = texture(tex_, uv);

    float luminance = dot(color, vec4(0.3, 0.6, 0.1, 0.0));

    out_FragColor = vec4(vec3(luminance), 1.0);
}
