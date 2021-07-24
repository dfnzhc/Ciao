#version 460

out vec4 FragColor;

in vec2 TexCoord;
in vec3 vNormal;
in vec3 worldPos;

//
uniform vec3 camPos;

// texture samplers
uniform sampler2D Tex_Diffuse1;
uniform sampler2D Tex_Emissive1;
uniform sampler2D Tex_Normal1;
uniform sampler2D Tex_LightMap1;
uniform sampler2D Tex_Unknown1;

uniform samplerCube Tex_EnvMap;

const float M_PI = 3.141592653589793;

struct PBRInfo
{
	float NdotL;                  // 法线和光线夹角
	float NdotV;                  // 法线和视线夹角
	float NdotH;                  // 法线和半向量夹角
	float LdotH;                  // 光线和半向量夹角
	float VdotH;                  // 视线和半向量夹角
	float perceptualRoughness;    // 粗糙值
	vec3 reflectance0;            // 全反射的颜色
	vec3 reflectance90;           // 零角度的反射颜色
	float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
	vec3 diffuseColor;            // 漫反射颜色
	vec3 specularColor;           // 镜面反射颜色
	vec3 n;						// 法线
	vec3 v;						// 视线，从表面到相机
};

// http://www.thetenthplanet.de/archives/1180
// 计算切线空间标架
mat3 cotangentFrame( vec3 N, vec3 p, vec2 uv );

// 采样切线空间法线
vec3 perturbNormal(vec3 n, vec3 v, vec3 normalSample, vec2 uv);

// 计算 IBL
vec3 calculatePBRInputsMetallicRoughness( vec4 albedo, vec3 normal, vec3 cameraPos, vec3 worldPos, vec4 mrSample, out PBRInfo pbrInputs);

// Calculation of the lighting contribution from an optional Image Based Light source.
vec3 getIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection);
// one hardcoded light source
vec3 calculatePBRLightContribution( inout PBRInfo pbrInputs, vec3 lightDirection, vec3 lightColor );

vec3 diffuseBurley(PBRInfo pbrInputs);

void main()
{
	vec4 Kao = texture(Tex_LightMap1, TexCoord);
	vec4 Ke = texture(Tex_Emissive1, TexCoord);
	vec4 Kd = texture(Tex_Diffuse1, TexCoord);
	vec2 MeR = texture(Tex_Unknown1, TexCoord).yz;

	vec3 n = normalize(vNormal);

	vec3 normalSample = texture(Tex_Normal1, TexCoord).xyz;

	n = perturbNormal(n, normalize(camPos - worldPos), normalSample, TexCoord);

	vec4 mrSample = texture(Tex_Unknown1, TexCoord);

	PBRInfo pbrInputs;
	Ke.rgb = pow(Ke.rgb, vec3(2.2));

	// 计算 IBL image-based lighting
	vec3 color = calculatePBRInputsMetallicRoughness(Kd, n, camPos.xyz, worldPos, mrSample, pbrInputs);
	// 计算光源颜色
	color += calculatePBRLightContribution( pbrInputs, normalize(vec3(-1.0, -1.0, -1.0)), vec3(2.0) );
	// ambient occlusion
	color = color * ( Kao.r < 0.01 ? 1.0 : Kao.r );
	// emissive
	color = pow( Ke.rgb + color, vec3(1.0/2.2) );

	FragColor = vec4(color, 1.0);
}

vec3 perturbNormal(vec3 n, vec3 v, vec3 normalSample, vec2 uv)
{
	vec3 map = normalize( 2.0 * normalSample - vec3(1.0) );
	mat3 TBN = cotangentFrame(n, v, uv);
	return normalize(TBN * map);
}

mat3 cotangentFrame( vec3 N, vec3 p, vec2 uv )
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );

	// solve the linear system
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );

	// calculate handedness of the resulting cotangent frame
	float w = (dot(cross(N, T), B) < 0.0) ? -1.0 : 1.0;

	// adjust tangent if needed
	T = T * w;

	return mat3( T * invmax, B * invmax, N );
}

vec3 calculatePBRInputsMetallicRoughness( vec4 albedo, vec3 normal, vec3 cameraPos, vec3 worldPos, vec4 mrSample, out PBRInfo pbrInputs)
{
	// Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
	float  perceptualRoughness = mrSample.g;
	float metallic = mrSample.b;

	const float c_MinRoughness = 0.04;

	perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
	metallic = clamp(metallic, 0.0, 1.0);

	// Roughness is authored as perceptual roughness; as is convention,
	// convert to material roughness by squaring the perceptual roughness [2].
	float alphaRoughness = perceptualRoughness * perceptualRoughness;

	vec4 baseColor = albedo;

	vec3 f0 = vec3(0.04);
	vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
	diffuseColor *= 1.0 - metallic;
	vec3 specularColor = mix(f0, baseColor.rgb, metallic);

	// Compute reflectance.
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	// For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
	// For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

	vec3 n = normalize(normal);					// normal at surface point
	vec3 v = normalize(cameraPos - worldPos);	// Vector from surface point to camera
	vec3 reflection = normalize(reflect(-v, n));

	pbrInputs.NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
	pbrInputs.perceptualRoughness = perceptualRoughness;
	pbrInputs.reflectance0 = specularEnvironmentR0;
	pbrInputs.reflectance90 = specularEnvironmentR90;
	pbrInputs.alphaRoughness = alphaRoughness;
	pbrInputs.diffuseColor = diffuseColor;
	pbrInputs.specularColor = specularColor;
	pbrInputs.n = n;
	pbrInputs.v = v;

	// Calculate lighting contribution from image based lighting source (IBL)
	vec3 color = getIBLContribution(pbrInputs, n, reflection);

	return color;
}

vec3 getIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection)
{
	vec3 specularLight = texture(Tex_EnvMap, reflection).rgb;
	vec3 specular = specularLight * pbrInputs.specularColor;

	vec3 diffuse = pbrInputs.diffuseColor;

	return diffuse + specular;
}

vec3 diffuseBurley(PBRInfo pbrInputs)
{
	float f90 = 2.0 * pbrInputs.LdotH * pbrInputs.LdotH * pbrInputs.alphaRoughness - 0.5;

	return (pbrInputs.diffuseColor / M_PI) * (1.0 + f90 * pow((1.0 - pbrInputs.NdotL), 5.0)) * (1.0 + f90 * pow((1.0 - pbrInputs.NdotV), 5.0));
}

vec3 calculatePBRLightContribution( inout PBRInfo pbrInputs, vec3 lightDirection, vec3 lightColor )
{
	vec3 n = pbrInputs.n;
	vec3 v = pbrInputs.v;
	vec3 l = normalize(lightDirection);		// Vector from surface point to light
	vec3 h = normalize(l+v);				// Half vector between both l and v

	float NdotV = pbrInputs.NdotV;
	float NdotL = clamp(dot(n, l), 0.001, 1.0);
	float NdotH = clamp(dot(n, h), 0.0, 1.0);
	float LdotH = clamp(dot(l, h), 0.0, 1.0);
	float VdotH = clamp(dot(v, h), 0.0, 1.0);

	pbrInputs.NdotL = NdotL;
	pbrInputs.NdotH = NdotH;
	pbrInputs.LdotH = LdotH;
	pbrInputs.VdotH = VdotH;

	// Calculate the shading terms for the microfacet specular shading model
	vec3 F = pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);

	float rSqr = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;

	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(rSqr + (1.0 - rSqr) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(rSqr + (1.0 - rSqr) * (NdotV * NdotV)));

	float G = attenuationL * attenuationV;

	float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
	float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;

	float D = roughnessSq / (M_PI * f * f);

	// Calculation of analytical lighting contribution
	vec3 diffuseContrib = (1.0 - F) * diffuseBurley(pbrInputs);
	vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);

	vec3 color = NdotL * lightColor * (diffuseContrib + specContrib);

	return color;
}