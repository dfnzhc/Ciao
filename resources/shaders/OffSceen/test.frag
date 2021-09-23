#version 460 core

layout (location=0) in vec2 uv;

layout (location=0) out vec4 out_FragColor;

layout(binding = 0) uniform sampler2D tex;

void main()
{
    out_FragColor = vec4(texture(tex, uv).rgb, 1.0);
}
