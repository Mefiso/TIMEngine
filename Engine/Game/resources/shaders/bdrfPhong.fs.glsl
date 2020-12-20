#version 440

in vec3 fragNormal;
in vec2 fragUV;
in vec3 worldPos;

out vec4 color;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 cameraPos;

uniform float shininess;

uniform sampler2D diffuse;
uniform sampler2D specular;

void main()
{
	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse
	vec3 colorDiffuse = texture(diffuse, fragUV).xyz;
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(lightDir);
	float NdotL = max(dot(normal, -lightDir), 0.0);

	// Specular
	vec3 specularMap = texture(specular, fragUV).xyz;
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