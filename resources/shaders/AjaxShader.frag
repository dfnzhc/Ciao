#version 460

out vec4 FragColor;

layout (location=0) in vec2 tc;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 worldPos;
layout (location=3) in vec4 FragPosLightSpace;

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

layout (binding=7) uniform sampler2D texShadow;

void main() 
{
    vec3 n = normalize(normal);

    vec3 v = normalize(camPos - worldPos);

    vec3 lightDir = vec3(0);
    float attenuation = 1.0;
    float intensity = 1.0;

	if (LightType == 0) {
		lightDir = normalize(-light.dir);
	}
	else {
        lightDir = normalize(light.pos - worldPos);
        float dist = length(light.pos - worldPos);
	    attenuation = 1.0 / (1.0f + 0.09 * dist + 0.032 * dist * dist);

        if (LightType == 2) {
            float ang = dot(lightDir, normalize(-light.dir));
            intensity = clamp((ang - light.cutoff) / (light.theta - light.cutoff), 0.0, 1.0);
        }
	}
	
    float diff = max(dot(n, lightDir), 0.0);

	vec3 halfwayDir = normalize(lightDir + v);
	float spec = pow(max(dot(halfwayDir, n), 0.0), 32);

    vec3 ambient = light.ambient * vec3(1.0);
	vec3 diffuse = light.diffuse * diff * vec3(1.0);
	vec3 specular = light.specular * spec * vec3(1.0);

    vec3 color = intensity * attenuation * (ambient + diffuse + specular);

	FragColor = vec4(color, 1.0);
}