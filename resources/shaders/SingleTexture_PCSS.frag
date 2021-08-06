#version 460

out vec4 FragColor;

layout (location=0) in vec2 tc;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 worldPos;
layout (location=3) in vec4 FragPosLightSpace;

layout(std140, binding = 0) uniform PerFrameData
{
	mat4 viewMatrix;
	mat4 projMatrix;
	vec4 cameraPos;
};

layout(std140, binding = 1) uniform Light
{
	vec4 pos;
	vec4 dir;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	
	float cutoff;
	float theta;
};

uniform int LightType;
uniform int wLight;

// texture samplers
layout (binding=1) uniform sampler2D texDiff;
layout (binding=2) uniform sampler2D texNorm;
layout (binding=7) uniform sampler2D texShadow;

// Shadow map related variables
#define NUM_SAMPLES 20
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10

#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586

highp float rand_1to1(highp float x ) {
	// -1 -1
	return fract(sin(x)*10000.0);
}

highp float rand_2to1(vec2 uv ) {
	// 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples( const in vec2 randomSeed ) {
	float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
	float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

	float angle = rand_2to1( randomSeed ) * PI2;
	float radius = INV_NUM_SAMPLES;
	float radiusStep = radius;

	for( int i = 0; i < NUM_SAMPLES; i ++ ) {
		poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
		radius += radiusStep;
		angle += ANGLE_STEP;
	}
}

void uniformDiskSamples( const in vec2 randomSeed ) {

	float randNum = rand_2to1(randomSeed);
	float sampleX = rand_1to1( randNum ) ;
	float sampleY = rand_1to1( sampleX ) ;

	float angle = sampleX * PI2;
	float radius = sqrt(sampleY);

	for( int i = 0; i < NUM_SAMPLES; i ++ ) {
		poissonDisk[i] = vec2( radius * cos(angle) , radius * sin(angle)  );

		sampleX = rand_1to1( sampleY ) ;
		sampleY = rand_1to1( sampleX ) ;

		angle = sampleX * PI2;
		radius = sqrt(sampleY);
	}
}

vec3 getNormalFromMap();

highp float findBlocker(vec2 uv) {
	float texSize = 1.0 / float( textureSize(texShadow, 0 ).x );
	
	uniformDiskSamples(uv);

	highp float depth = 0.0;
	for( int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i ) {
		depth += texture( texShadow, uv + poissonDisk[i] * texSize ).r;
	}

	depth /= BLOCKER_SEARCH_NUM_SAMPLES;
	
	return depth;
}

float PCSS(){
	vec4 shadowCoords = FragPosLightSpace / FragPosLightSpace.w;
	float texSize = 1.0 / float( textureSize(texShadow, 0 ).x );

	if (shadowCoords.z > 0.0 && shadowCoords.z < 1.0) {
		float depth = shadowCoords.z - 0.001;
		// STEP 1: avgblocker depth
		float avgDep = findBlocker(shadowCoords.xy);
		
		float zReciver = 2.0 * depth - 1.0;
		float zBlocker = 2.0 * avgDep - 1.0;
		
		// STEP 2: penumbra size
		float filterSize = wLight * abs(zReciver - zBlocker) / zBlocker;

		// STEP 3: filtering
		poissonDiskSamples(shadowCoords.xy);
		float shadow = 0.0;
		for( int i = 0; i < NUM_SAMPLES;  ++i ) {
			shadow += (depth <= texture( texShadow, shadowCoords.xy + poissonDisk[i] * texSize * filterSize ).r ? 1.0 : 0.0);
		}
		shadow /= NUM_SAMPLES;

		return shadow;//mix(0.3, 1.0, shadow);
	}

	return 0.0;
	
	
}


float PCF() {
	vec4 shadowCoords = FragPosLightSpace / FragPosLightSpace.w;
	float texSize = 1.0 / float( textureSize(texShadow, 0 ).x );
	
	if (shadowCoords.z > 0.0 && shadowCoords.z < 1.0) {
		poissonDiskSamples(shadowCoords.xy);

		float shadow = 0.0;
		float depth = shadowCoords.z - 0.001;
		for( int i = 0; i < NUM_SAMPLES;  ++i ) {
			shadow += (depth < texture( texShadow, shadowCoords.xy + poissonDisk[i] * texSize ).r ? 1.0 : 0.0);
		}
		shadow /= NUM_SAMPLES;
		
		return mix(0.3, 1.0, shadow);
	}

	return 0.0;
	
}

float useShadowMap(){
	vec4 shadowCoords = FragPosLightSpace / FragPosLightSpace.w;
	
	if (shadowCoords.z > 0.0 && shadowCoords.z < 1.0) {
		float sampleDepth = texture(texShadow, shadowCoords.xy).r;
		
		if (shadowCoords.z - 0.001 < sampleDepth)
			return 1.0;
	}

	return 0.0;
}

vec3 blinnPhong() {
	vec3 albedo = texture(texDiff, tc).rgb;
	albedo = pow(albedo, vec3(2.2));
	
	vec3 ambientCol = 0.05 * albedo;
	
	vec3 lightDir = normalize(pos.xyz);
	vec3 normal = getNormalFromMap();
	float diff = max(dot(lightDir, normal), 0.0);
	float dist = length(pos.xyz - worldPos);
	vec3 atten_coff = vec3(1.0) / (1.0f + 0.09 * dist + 0.032 * dist * dist);
	vec3 diffuseCol = diff * atten_coff * albedo;
	
	vec3 viewDir = normalize(cameraPos.xyz - worldPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfDir, normal), 0.0), 32.0);
	vec3 specularCol = atten_coff * spec;
	
	vec3 radiance = (ambientCol + diffuseCol + specularCol);
	vec3 phongColor = pow(radiance, vec3(1.0 / 2.2));
	
	return phongColor;
}

void main()
{
	float visibility = 1.0;
	//visibility = useShadowMap();
	//visibility = PCF();
	visibility = PCSS();
	
	vec3 phongColor = blinnPhong();

	
	FragColor = vec4(phongColor * visibility, 1.0);
}

vec3 getNormalFromMap()
{
	vec3 tangentNormal = texture(texNorm, tc).xyz * 2.0 - 1.0;

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