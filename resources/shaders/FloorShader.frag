#version 460

out vec4 FragColor;

layout (location=0) in vec2 tc;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 worldPos;

struct Light
{
	vec3 pos;
	vec3 dir;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float cutoff;
	float theta;
};

uniform int LightType;
uniform vec3 camPos;
uniform Light light;

// texture samplers
layout (binding=0) uniform sampler2D texDiff;
layout (binding=1) uniform sampler2D texDisp;
layout (binding=2) uniform sampler2D texRough;
layout (binding=3) uniform sampler2D texNor_gl;

// http://www.thetenthplanet.de/archives/1180
// modified to fix handedness of the resulting cotangent frame
mat3 cotangentFrame( vec3 N, vec3 p, vec2 uv );

vec3 perturbNormal(vec3 n, vec3 v, vec3 normalSample, vec2 uv);

vec3 CalDirLightColor(vec3 N, vec3 V);
vec3 CalPointLightColor(vec3 N, vec3 V);
vec3 CalSpotLightColor(vec3 N, vec3 V);

vec3 getNormalFromMap();

void main()
{
	vec3 n = normalize(normal);
	vec3 tnormal = texture(texNor_gl, tc).xyz;
	
	n = getNormalFromMap();//normalize(perturbNormal(n, normalize(camPos - worldPos), tnormal, tc));
	
	vec3 v = normalize(camPos - worldPos);

	vec3 color = vec3(0);
	if (LightType == 0) {
		color += CalDirLightColor(normal, v);
	}
	else if (LightType == 1) {
		color += CalPointLightColor(normal, v);
	}
	else if (LightType == 2) {
		color += CalSpotLightColor(normal, v);
	}
	

	FragColor = vec4(color, 1.0);
	//FragColor = vec4(texture(texDiff, tc).xyz, 1.0);
}

// http://www.thetenthplanet.de/archives/1180
// modified to fix handedness of the resulting cotangent frame
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

vec3 perturbNormal(vec3 n, vec3 v, vec3 normalSample, vec2 uv)
{
	vec3 map = normalize( 2.0 * normalSample - vec3(1.0) );
	mat3 TBN = cotangentFrame(n, v, uv);
	return normalize(TBN * map);
}

vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(texNor_gl, tc).xyz * 2.0 - 1.0;

	vec3 Q1  = dFdx(worldPos);
	vec3 Q2  = dFdy(worldPos);
	vec2 st1 = dFdx(tc);
	vec2 st2 = dFdy(tc);

	vec3 N   = normalize(normal);
	vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B  = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

vec3 CalDirLightColor(vec3 N, vec3 V) {
	vec3 lightDir = normalize(-light.dir);
	
	float diff = max(dot(N, lightDir), 0.0);
	
	vec3 halfwayDir = normalize(lightDir + V);
	float spec = pow(max(dot(halfwayDir, N), 0.0), 16);
	
	vec3 albedo = vec3(texture(texDiff, tc));
	
	vec3 ambient = light.ambient * albedo;
	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * vec3(1.0);
	
	return (ambient + diffuse + specular);
}

vec3 CalPointLightColor(vec3 N, vec3 V) 
{
	vec3 lightDir = normalize(light.pos - worldPos);

	float diff = max(dot(N, lightDir), 0.0);

	vec3 halfwayDir = normalize(lightDir + V);
	float spec = pow(max(dot(halfwayDir, N), 0.0), 32);
	
	float dist = length(light.pos - worldPos);
	float attenuation = 1.0 / (1.0f + 0.09 * dist + 0.032 * dist * dist);

	vec3 albedo = vec3(texture(texDiff, tc));

	vec3 ambient = light.ambient * albedo;
	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * vec3(1.0);

	return attenuation * (ambient + diffuse + specular);
}

vec3 CalSpotLightColor(vec3 N, vec3 V) {
	vec3 lightDir = normalize(light.pos - worldPos);
	
	float ang = dot(lightDir, normalize(-light.dir));

	vec3 albedo = vec3(texture(texDiff, tc));
	
//	if (ang < light.cutoff) {
		float diff = max(dot(N, lightDir), 0.0);

		vec3 halfwayDir = normalize(lightDir + V);
		float spec = pow(max(dot(halfwayDir, N), 0.0), 32);

		float dist = length(light.pos - worldPos);
		float attenuation = 1.0 / (1.0f + 0.09 * dist + 0.032 * dist * dist);
		
		float intensity = clamp((ang - light.cutoff) / (light.theta - light.cutoff), 0.0, 1.0);
		
		vec3 ambient = intensity * light.ambient * albedo;
		vec3 diffuse = intensity * light.diffuse * diff * albedo;
		vec3 specular = light.specular * spec * vec3(1.0);

		return attenuation * (ambient + diffuse + specular);
//	}
//	else {
//		return light.ambient * albedo;
//	}
}