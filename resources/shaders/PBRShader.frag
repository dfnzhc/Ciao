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

const float PI = 3.141592653589793;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(Tex_Normal1, TexCoord).xyz * 2.0 - 1.0;

	vec3 Q1  = dFdx(worldPos);
	vec3 Q2  = dFdy(worldPos);
	vec2 st1 = dFdx(TexCoord);
	vec2 st2 = dFdy(TexCoord);

	vec3 N   = normalize(vNormal);
	vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B  = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

void main()
{
	float AO = texture(Tex_LightMap1, TexCoord).r;
	vec3 Le = texture(Tex_Emissive1, TexCoord).rgb;
	vec3 albedo = pow(texture(Tex_Diffuse1, TexCoord).rgb, vec3(2.2));
	float roughness = texture(Tex_Unknown1, TexCoord).y;
	float metallic = texture(Tex_Unknown1, TexCoord).z;
	
	// 计算切线空间法线
	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - worldPos);
	
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	vec3 Lo = vec3(0.0);
	
	vec3 lightPos[3] = {vec3(3.0, 4.0, 5.0), vec3(7.0, 8.0, 9.0), vec3(0.0, 0.0, 10.0)};
	vec3 lightColor = vec3(150.0);
	
	for (int i = 0; i < 3; ++i) {
		vec3 L = normalize(lightPos[i] - worldPos);
		vec3 H = normalize(L + V);
		float distance = length(lightPos[i] - worldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColor * attenuation;

		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 numerator    = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;
		float NdotL = max(dot(N, L), 0.0);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	
	vec3 ambient = vec3(0.03) * albedo * AO;

	vec3 color = ambient + Lo + Le;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}

