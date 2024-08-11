#version 460 core

in vec3 fColour;
in vec2 fTexCoords;
in vec3 fNormals;
in vec3 fVertPos;
in vec3 fViewPos;
in mat3 fTBN;

struct Material {
	sampler2D baseColourMap;
	sampler2D specularMap;
	sampler2D normalMap;
	float shininess;
	float specularStrength;
};

uniform Material material;

struct DirLight {
	vec3 colour;
	vec3 ambient;
	vec3 direction;
	float intensity;
};

struct PointLight {
	vec3 colour;
	vec3 position;
	float intensity;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 colour;
	vec3 position;
	vec3 direction;
	float cutOff;
	float linear;
	float quadratic;
	float intensity;
	float outerCutOff;
};

#define NUM_DIR_LIGHTS 1
uniform DirLight dirLights[NUM_DIR_LIGHTS];

#define NUM_POINT_LIGHTS 20
uniform PointLight pointLights[NUM_POINT_LIGHTS];

#define NUM_SPOT_LIGHTS 20
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];

out vec4 finalColour;

void main() {
	vec3 result = vec3(0.0f);
	vec3 baseColour = texture(material.baseColourMap, fTexCoords).rgb * fColour;
	vec3 specularColour = texture(material.specularMap, fTexCoords).rgb;
	vec3 viewDir = normalize(fViewPos - fVertPos);

	// Normal mapping
	vec3 normalColour = texture(material.normalMap, fTexCoords).rgb;
	normalColour = normalColour * 2.0f - 1.0f;
	// normalColour = normalize(fTBN * normalColour);

	// Directional lights
	for (int i = 0; i < NUM_DIR_LIGHTS; ++i) {
		vec3 lightDir = normalize(-dirLights[i].direction);
		vec3 reflectDir = reflect(-lightDir, fNormals);
		float colourIntensity = max(dot(fNormals, lightDir), 0.0f);

		vec3 ambientLight = max(dirLights[i].ambient, baseColour);
		vec3 lightColour = dirLights[i].colour * colourIntensity * dirLights[i].intensity;

		float specPower = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
		vec3 specular = specularColour * specPower * material.specularStrength;

		result += (ambientLight + lightColour + specular);
	}

	// Point lights
	for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
		vec3 lightDir = normalize(pointLights[i].position - fVertPos);
		vec3 reflectDir = reflect(-lightDir, fNormals);
		float diff = max(dot(fNormals, lightDir), 0.0f);

		float distance = length(pointLights[i].position - fVertPos);
		float attenuation = 1.0f / (1.0f + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));

		vec3 lightColour = pointLights[i].colour * diff * attenuation * pointLights[i].intensity;

		float specPower = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
		vec3 specular = specularColour * specPower * material.specularStrength;

		result += (lightColour + specular);
	}

	// Spot lights
	for (int i = 0; i < NUM_SPOT_LIGHTS; ++i) {
		vec3 lightDir = normalize(spotLights[i].position - fVertPos);
		float theta = dot(lightDir, normalize(-spotLights[i].direction));

		if (theta > spotLights[i].outerCutOff) {
			float epsilon = (spotLights[i].cutOff - spotLights[i].outerCutOff);
			float fadeOutIntensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0f, 1.0f) * spotLights[i].intensity;

			vec3 reflectDir = reflect(-lightDir, fNormals);
			float diff = max(dot(fNormals, lightDir), 0.0f);

			float distance = length(spotLights[i].position - fVertPos);
			float attenuation = 1.0f / (1.0f + spotLights[i].linear * distance + spotLights[i].quadratic * (distance * distance));

			vec3 lightColour = spotLights[i].colour * diff * attenuation * fadeOutIntensity;

			float specPower = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
			vec3 specular = specularColour * specPower * material.specularStrength;

			result += (lightColour);
		}
	}

	finalColour = vec4(result, 1.0f);
}
