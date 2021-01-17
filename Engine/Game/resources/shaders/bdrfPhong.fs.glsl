#version 440

in vec3 fragNormal;
in vec2 fragUV;
in vec3 worldPos;

out vec4 color;

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

struct DirLight
{
	vec3 direction;
	vec3 color;
	float intensity;
};

struct PointLight
{
	vec3 position;
	vec3 color;
	float intensity;
	float Kc;
	float Kl;
	float Kq;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float Kc;
	float Kl;
	float Kq;
	float innerAng;
	float outerAng;
};

struct Lights
{
	vec3 ambientColor;
	int hasDirectional;
	DirLight directional;
	int num_point;
	PointLight pointLights[8];
	int num_spot;
	SpotLight spotLights[8];
};
uniform Lights lights;
// ------------- LIGHT PROCESSING --------------- //
// Directional Light
vec3 ProcessDirectional(DirLight LDir, vec3 colorDiffuse, vec3 specularMap, float shininess, vec3 normal, vec3 viewDir, vec3 Rf0)
{
	vec3 lightDir = normalize(LDir.direction);

	float NdotL = max(dot(normal, -lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflect(lightDir, normal)), 0.0), shininess);

	vec3 Rfi = Rf0 + (1-Rf0) * pow((1-NdotL),5);

	vec3 Li = LDir.color * LDir.intensity;

	return (colorDiffuse*(1-Rf0) + (shininess+2)/2 * Rfi * spec * specularMap) * Li * NdotL;
}

// Point Lights
vec3 ProcessPoint(PointLight LPoint, vec3 colorDiffuse, vec3 specularMap, float shininess, vec3 normal, vec3 viewDir, vec3 Rf0)
{
	vec3 lightDir = normalize(worldPos - LPoint.position); // O al reves?
	float d = distance(LPoint.position, worldPos);

	float NdotL = max(dot(normal, -lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflect(lightDir, normal)), 0.0), shininess);

	vec3 Rfi = Rf0 + (1-Rf0) * pow((1-NdotL),5);

	float attenuation = 1 / (LPoint.Kc + LPoint.Kl *  + LPoint.Kq *d*d);	
	vec3 Li = LPoint.color * LPoint.intensity * attenuation;

	return (colorDiffuse*(1-Rf0) + (shininess+2)/2 * Rfi * spec * specularMap) * Li * NdotL;
}

// Spot Lights
vec3 ProcessSpot(SpotLight LSpot, vec3 colorDiffuse, vec3 specularMap, float shininess, vec3 normal, vec3 viewDir, vec3 Rf0)
{
	vec3 lightDir = normalize(worldPos - LSpot.position); // O al reves?
	float d = distance(LSpot.position, worldPos);

	float NdotL = max(dot(normal, -lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflect(lightDir, normal)), 0.0), shininess);

	vec3 Rfi = Rf0 + (1-Rf0) * pow((1-NdotL),5);

	float incidenceAngle = dot(lightDir, LSpot.direction);
	float cosInner = cos(LSpot.innerAng);
	float cosOuter = cos(LSpot.outerAng);

	float angleAttenuation;
	angleAttenuation = incidenceAngle > cosInner ? 1 : ((cosInner > incidenceAngle && incidenceAngle > cosOuter) ? ((incidenceAngle - cosOuter)/(cosInner - cosOuter)) : 0);
	float attenuation = 1 / (LSpot.Kc + LSpot.Kl *  + LSpot.Kq *d*d);	
	vec3 Li = LSpot.color * LSpot.intensity * attenuation * angleAttenuation;

	return (colorDiffuse*(1-Rf0) + (shininess+2)/2 * Rfi * spec * specularMap) * Li * NdotL;
}

// ------------------- MAIN --------------------- //
void main()
{
	vec3 normal = normalize(fragNormal);
	vec3 viewDir = normalize(cameraPos-worldPos);
	float shininess = material.shininessAlpha > 0 ?  texture(material.specular1, fragUV).a : material.shininess;
	vec3 Rf0 = vec3(0.08,0.08,0.08); //TODO: this should come as parameter?   ---- //vec3 Rf0 = vec3(0.56,0.57,0.58);

	// Diffuse Color
	vec3 colorDiffuse = material.hasDiffuseMap > 0 ? texture(material.diffuse1, fragUV).rgb : material.diffuse;
	// Specular Color
	vec3 specularMap = material.hasSpecularMap > 0 ? texture(material.specular1, fragUV).rgb : material.specular;

	// Ambient Light
	vec3 Fcolor = lights.ambientColor * colorDiffuse;
	// Directional Light
	if (lights.hasDirectional == 1)
		Fcolor += ProcessDirectional(lights.directional, colorDiffuse, specularMap, shininess, normal, viewDir, Rf0);
	// Point Lights
	for (int i = 0; i<lights.num_point; ++i)
		Fcolor += ProcessPoint(lights.pointLights[i], colorDiffuse, specularMap, shininess, normal, viewDir, Rf0);
	// Spot Lights
	for (int i = 0; i<lights.num_spot; ++i)
		Fcolor += ProcessSpot(lights.spotLights[i], colorDiffuse, specularMap, shininess, normal, viewDir, Rf0);

	// Tone Mapping - Reinhard
	vec3 ldr = Fcolor / (Fcolor + vec3(1.0)); // reinhard tone mapping
	// Gamma correction
	ldr = pow(ldr, vec3(1/2.2));
	color = vec4(Fcolor, 1.0);
}