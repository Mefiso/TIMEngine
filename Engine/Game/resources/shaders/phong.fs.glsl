#version 440

in vec3 fragNormal;
in vec2 fragUV;
in vec3 fragPosWorld;

out vec4 color;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 cameraPos;

uniform float shininess;
uniform sampler2D diffuse;

void main()
{
	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse
	vec3 lightDir = normalize(lightDir);
	vec3 colorDiffuse = texture(diffuse, fragUV).xyz;
	vec3 normal = normalize(fragNormal);
	float NdotL = max(dot(normal, -lightDir), 0.0);
	vec3 diff = NdotL * lightColor;

	// Specular
	vec3 viewDir = normalize(cameraPos - fragPosWorld);
	vec3 reflected = reflect(lightDir, normal);
	float spec = pow(max(dot(viewDir, reflected), 0.0), shininess);
	vec3 specular = spec * lightColor;

	color = vec4((ambient + 0.5 * diff) * colorDiffuse +  0.5 * specular, 1.0);
}