#version 460

out vec4 FragColor;

layout (location=0) in vec2 tc;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 worldPos;
layout (location=3) in vec4 FragPosLightSpace;

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
uniform vec3 camPos;

layout (binding=7) uniform sampler2D texShadow;

void main() 
{
    vec3 n = normalize(normal);

    vec3 v = normalize(camPos - worldPos);

    vec3 lightDir = vec3(0);
    float attenuation = 1.0;
    float intensity = 1.0;

	if (LightType == 0) {
		lightDir = normalize(-dir.xyz);
	}
	else {
        lightDir = normalize(pos.xyz - worldPos);
        float dist = length(pos.xyz - worldPos);
	    attenuation = 1.0 / (1.0f + 0.09 * dist + 0.032 * dist * dist);

        if (LightType == 2) {
            float ang = dot(lightDir, normalize(-dir.xyz));
            intensity = clamp((ang - cutoff) / (theta - cutoff), 0.0, 1.0);
        }
	}
	
    float diff = max(dot(n, lightDir), 0.0);

	vec3 halfwayDir = normalize(lightDir + v);
	float spec = pow(max(dot(halfwayDir, n), 0.0), 32);

    vec3 ambientCol = ambient.rgb * vec3(1.0);
	vec3 diffuseCol = diffuse.rgb * diff * vec3(1.0);
	vec3 specularCol = specular.rgb * spec * vec3(1.0);

    vec3 color = intensity * attenuation * (ambientCol + diffuseCol + specularCol);

	FragColor = vec4(color, 1.0);
}