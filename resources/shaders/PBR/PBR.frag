#version 460 core

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projMatrix;
    vec4 cameraPos;
};

layout (location=0) in vec2 uv;
layout (location=1) in vec3 wpo;
layout (location=2) in vec3 normal;

layout (location=0) out vec4 out_FragColor;

layout (binding = 0) uniform sampler2D tex_Diff;    // albedo
layout (binding = 1) uniform sampler2D tex_AO;      // ambient occlusion
layout (binding = 2) uniform sampler2D tex_Em;      // emissive
layout (binding = 3) uniform sampler2D tex_Me_R;     // metal and roughness
layout (binding = 4) uniform sampler2D tex_Nor;     // normal

layout (binding = 5) uniform samplerCube texEnvMap;          
layout (binding = 6) uniform samplerCube texEnvMapIrradiance;
layout (binding = 7) uniform sampler2D texBRDF_LUT;          

#include "../Common.glsl"
#include "pbrCommon.glsl"

const vec3 lightDir     = vec3(-1, -1, -1);
const vec3 lightColor   = vec3(1, 1, 1);

void main()
{
    vec4 Kd  = SRGBtoLINEAR(texture(tex_Diff, uv));
    vec4 Ke  = SRGBtoLINEAR(texture(tex_Em, uv));
    vec4 Kao = texture(tex_AO, uv);
    // 粗糙度存储在 g 通道，金属度存储在 b 通道，r 通道为遮罩数据保留（可选）
    vec2 MeR = texture(tex_Me_R, uv).yz;
    vec3 normalSample = texture(tex_Nor, uv).xyz;

    PBRInfo pbrInputs;
    
    // world-space normal
    vec3 n = normalize(normal);
    vec3 v = normalize(cameraPos.xyz - wpo);

    // normal mapping
    n = perturbNormal(normalSample, n, v, uv);
    
    float perceptualRoughness = MeR.x;
    float metallic = MeR.y;

    perceptualRoughness = clamp(perceptualRoughness, 0.04, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    // 粗糙度是按照人类感官来创建的，所以按照惯例，需要通过平方把它转换成材料粗糙度 [2]
    float alphaRoughness = perceptualRoughness * perceptualRoughness;
    
    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = Kd.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;
    vec3 specularColor = mix(f0, Kd.rgb, metallic);
    
    // 计算反射率
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // 对典型的入射反射率范围 (between 4% to 100%)， 将掠射的反射率设为 100%，以便得到典型的菲涅尔效应
    // 当漫反射程度很高的物体有很低的反射率时 (below 4%), 逐步将掠射的反射率降为 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

    vec3 l = normalize(lightDir);
    vec3 h = normalize(l + v);
    vec3 reflection = -normalize(reflect(v, n));

    pbrInputs.NdotL = clamp(dot(n, l), 0.001, 1.0);
    pbrInputs.NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
    pbrInputs.NdotH = clamp(dot(n, h), 0.0, 1.0);
    pbrInputs.LdotH = clamp(dot(l, h), 0.0, 1.0);
    pbrInputs.VdotH = clamp(dot(v, h), 0.0, 1.0);
    pbrInputs.perceptualRoughness = perceptualRoughness;
    pbrInputs.metalness = metallic;
    pbrInputs.reflectance0 = specularEnvironmentR0;
    pbrInputs.reflectance90 = specularEnvironmentR90;
    pbrInputs.alphaRoughness = alphaRoughness;
    pbrInputs.diffuseColor = diffuseColor;
    pbrInputs.specularColor = specularColor;

    // 计算微表面反射模型的各项
    vec3 F = specularReflection(pbrInputs);
    float G = geometricOcclusion(pbrInputs);
    float D = microfacetDistribution(pbrInputs);

    // 分析照明度的贡献
    vec3 diffuseContrib = (1.0 - F) * diffuseBurley(pbrInputs);
    vec3 specContrib = F * G * D / (4.0 * pbrInputs.NdotL * pbrInputs.NdotV);
    // 获得最终的光照强度，用光照的余弦进行缩放
    vec3 color = pbrInputs.NdotL * lightColor * (diffuseContrib + specContrib);

    // 计算 IBL 的光照贡献
    color += getIBLContribution(pbrInputs, n, reflection);

    // 施加环境光遮蔽
    color = color * ( Kao.r < 0.01 ? 1.0 : Kao.r );
    
    // 自发光
    color += Ke.rgb;
    
    out_FragColor = vec4(pow(color, vec3(1.0 / 2.2)), Kd.a);
}
