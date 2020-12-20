#version 440

in vec3 fragNormal;
in vec2 fragUV;
in vec3 fragPosWorld;

out vec4 color;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 cameraPos;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	sampler2D diffuse1;
	float shininess;

	int hasDiffuseMap;
};

uniform Material material;

void main()
{
	// Ambient
	vec3 ambient = material.ambient * lightColor;

	// Diffuse
	vec3 lightDir = normalize(lightDir);
	vec3 colorDiffuse;
	if (material.hasDiffuseMap > 0)
	{
		colorDiffuse = texture(material.diffuse1, fragUV).xyz;
	}
	else {
		colorDiffuse = material.diffuse;
	}
	vec3 normal = normalize(fragNormal);
	float NdotL = max(dot(normal, -lightDir), 0.0);
	vec3 diff = NdotL * lightColor;

	// Specular
	vec3 viewDir = normalize(cameraPos - fragPosWorld);
	vec3 reflected = reflect(lightDir, normal);
	float spec = pow(max(dot(viewDir, reflected), 0.0), material.shininess);
	vec3 specular = spec * lightColor;

	vec3 Fcolor = (ambient + 0.5 * diff) * colorDiffuse +  0.5 * specular;
	// gamma correction
	Fcolor = pow(Fcolor, vec3(1/2.2));

	color = vec4(Fcolor, 1.0);
}