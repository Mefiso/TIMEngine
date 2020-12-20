#version 440

in vec3 fragNormal;
in vec2 fragUV;
in vec3 worldPos;

out vec4 color;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 cameraPos;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	sampler2D diffuse1;
	vec3 specular;
	sampler2D specular1;
	float shininess;

	int hasDiffuseMap;
	int hasSpecularMap;
	int shininessAlpha;
};

uniform Material material;

void main()
{
	// Ambient
	vec3 ambient = material.ambient * lightColor;

	// Diffuse
	vec3 colorDiffuse;
	if(material.hasDiffuseMap > 0)
	{
		colorDiffuse = texture(material.diffuse1, fragUV).rgb;
	}
	else
	{
		colorDiffuse = material.diffuse;
	}
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(lightDir);
	float NdotL = max(dot(normal, -lightDir), 0.0);

	// Specular
	vec3 specularMap;
	if (material.hasSpecularMap > 0)
	{
		specularMap = texture(material.specular1, fragUV).rgb;
	}
	else
	{
		specularMap = material.specular;
	}
	float shininess = material.shininessAlpha > 0 ?  texture(material.specular1, fragUV).a : material.shininess;
	vec3 viewDir = normalize(cameraPos-worldPos);
	vec3 reflected = reflect(lightDir, normal);
	float spec = pow(max(dot(viewDir, reflected), 0.0), shininess);

	//vec3 Rf0 = vec3(0.56,0.57,0.58); //this should come as parameter
	vec3 Rf0 = vec3(0.08,0.08,0.08); //this should come as parameter
	vec3 Rfi = Rf0 + (1-Rf0) * pow((1-NdotL),5);

	vec3 Fcolor = ambient * colorDiffuse + (colorDiffuse*(1-Rf0) + (shininess+2)/2 * Rfi * spec * specularMap) * lightColor * NdotL;
	// Gamma correction
	Fcolor = pow(Fcolor, vec3(1/2.2));

	color = vec4(Fcolor, 1.0);
}