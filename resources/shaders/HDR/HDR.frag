#version 460 core

layout (location=0) in vec2 uv;

layout (location=0) out vec4 out_FragColor;

layout(binding = 0) uniform sampler2D texScene_;
layout(binding = 1) uniform sampler2D texLuminance_;
layout(binding = 2) uniform sampler2D texBloom_;

layout(std140, binding = 0) uniform HDRParams
{
    float exposure;
    float maxWhite;
    float bloomStrength;
};

// Extended Reinhard tone mapping operator
vec3 Reinhard2(vec3 x)
{
    return (x * (1.0 + x / (maxWhite * maxWhite))) / (1.0 + x);
}

void main()
{
    vec3 color = texture(texScene_, uv).rgb;
    vec3 bloom = texture(texBloom_, uv).rgb;
    float avgLuminance = texture(texLuminance_, vec2(0.5, 0.5)).x;

    float midGray = 0.5;

    color *= exposure * midGray / (avgLuminance + 0.001);
    color = Reinhard2(color);

    out_FragColor = vec4(color + bloomStrength * bloom, 1.0);
}
