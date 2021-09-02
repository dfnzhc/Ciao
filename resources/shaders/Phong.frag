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

uniform vec3 lightPos;
uniform int withTexture;
uniform int shadowFunc;
uniform int sampleCount;
uniform int filterSize;
uniform float bias;
uniform int wLight;

// texture samplers
layout (binding=1) uniform sampler2D texDiff;
layout (binding=7) uniform sampler2D texShadow;

// Shadow map related variables
#define NUM_SAMPLES 100
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
	float ANGLE_STEP = PI2 * float( sampleCount ) / float( sampleCount );
	float INV_NUM_SAMPLES = 1.0 / float( sampleCount );

	float angle = rand_2to1( randomSeed ) * PI2;
	float radius = INV_NUM_SAMPLES;
	float radiusStep = radius;

	for( int i = 0; i < sampleCount; i ++ ) {
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

	for( int i = 0; i < sampleCount; i ++ ) {
		poissonDisk[i] = vec2( radius * cos(angle) , radius * sin(angle)  );

		sampleX = rand_1to1( sampleY ) ;
		sampleY = rand_1to1( sampleX ) ;

		angle = sampleX * PI2;
		radius = sqrt(sampleY);
	}
}

highp float findBlocker(vec2 uv) {
	float texSize = 1.0 / float( textureSize(texShadow, 0 ).x );

	uniformDiskSamples(uv);

	highp float depth = 0.0;
	for( int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i ) {
		depth += texture( texShadow, uv + poissonDisk[i] * 7 * texSize ).r;
	}

	depth /= BLOCKER_SEARCH_NUM_SAMPLES;

	return depth;
}

float PCSS(sampler2D shadowMap, vec3 shadowCoord){
	float texSize = 1.0 / float( textureSize(texShadow, 0 ).x );

	if (shadowCoord.z > 0.0 && shadowCoord.z < 1.0) {
		float depth = shadowCoord.z - 0.001;
		// STEP 1: avgblocker depth
		float avgDep = findBlocker(shadowCoord.xy);

		float zReciver = 2.0 * depth - 1.0;
		float zBlocker = 2.0 * avgDep - 1.0;

		// STEP 2: penumbra size
		float filterSize1 = wLight * abs(zReciver - zBlocker) / zBlocker;

		// STEP 3: filtering
		poissonDiskSamples(shadowCoord.xy);
		float shadow = 0.0;
		for( int i = 0; i < sampleCount;  ++i ) {
			shadow += (depth <= texture( shadowMap, shadowCoord.xy + poissonDisk[i] * texSize * filterSize1 ).r ? 1.0 : 0.0);
		}
		shadow /= sampleCount;

		return mix(0.1, 1.0, shadow);
	}

	return 0.0;
}


float PCF(sampler2D shadowMap, vec3 shadowCoord) {
	//float texSize = 1.0 / float(textureSize(shadowMap, 0).x);
	vec2 texSize = 1.0 / textureSize(shadowMap, 0);

	uniformDiskSamples(shadowCoord.xy);

	float shadow = 0.0;
	float depth = shadowCoord.z - bias;
	int range = filterSize / 2;
//	for ( int v=-range; v<=range; v++ ) for ( int u=-range; u<=range; u++ )
//		shadow += (depth < texture( shadowMap, shadowCoord.xy + texSize * vec2(u, v) ).r) ? 1.0 : 0.0;
	
	for( int i = 0; i < sampleCount;  ++i ) {
		shadow += (depth < texture( shadowMap, shadowCoord.xy + filterSize * texSize * poissonDisk[i] ).r) ? 1.0 : 0.0;
	}
	
	shadow /= sampleCount;
	
	//shadow /= (filterSize * filterSize);

	return shadow;
	
	
	return 0.0;
}

float useShadowMap(sampler2D shadowMap, vec3 shadowCoord){
	float currDepth = texture(shadowMap, shadowCoord.xy).r;
	
	if (shadowCoord.z < currDepth + 0.001)
		return 1.0;

	return 0.0;
}

vec3 blinnPhong() {
	vec3 albedo = vec3(1);
	if (withTexture == 1) {
		albedo = pow(texture(texDiff, tc).rgb, vec3(2.2));
	}
	
	vec3 ambientCol = 0.05 * albedo;
	
	vec3 lightDir = normalize(lightPos.xyz - worldPos);
	vec3 n = normalize(normal);
	float diff = max(dot(lightDir, n), 0.0);
	float dist = length(lightPos.xyz - worldPos);
	vec3 atten_coff = vec3(1.0) ;/// (1.0f + 0.09 * dist + 0.032 * dist * dist);
	vec3 diffuseCol = diff * atten_coff * albedo;
	
	vec3 viewDir = normalize(cameraPos.xyz - worldPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfDir, n), 0.0), 32.0);
	vec3 specularCol = atten_coff * spec;
	
	vec3 phongColor = (ambientCol + diffuseCol + specularCol);
	
	if (withTexture == 1) {
		phongColor = pow(phongColor, vec3(1.0 / 2.2));
	}
	
	return phongColor;
}

void main()
{
	float visibility = 1.0;
	vec3 shadowCoord = FragPosLightSpace.xyz / FragPosLightSpace.w;
	shadowCoord = 0.5 * shadowCoord + 0.5;
	
	if (shadowFunc == 0) {
		visibility = useShadowMap(texShadow, shadowCoord);
	}
	else if (shadowFunc == 1) {
		visibility = PCF(texShadow, FragPosLightSpace);
	}
	else if (shadowFunc == 2) {
		visibility = PCSS(texShadow, shadowCoord);
	}
	//visibility = PCSS();
	
	vec3 phongColor = blinnPhong();

	
	FragColor = vec4(phongColor * visibility, 1.0);
}