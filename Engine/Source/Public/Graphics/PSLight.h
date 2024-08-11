#pragma once

// External Headers
#include <GLM/vec3.hpp>

struct PSLight
{
	PSLight()
	{
		colour = glm::vec3(1.0f);
		intensity = 1.0;
	}
	virtual ~PSLight() = default;

	glm::vec3 colour;
	float intensity;
};

struct PSDirLight : public PSLight
{
	PSDirLight()
	{
		ambient = glm::vec3(0.01f);
		direction = glm::vec3(0.0f);
	}

	glm::vec3 ambient;
	glm::vec3 direction;
};

struct PSPointLight : public PSLight
{
	PSPointLight()
	{
		position = glm::vec3(0.0f);
		linear = 0.045f;
		quadratic = 0.0075f;
	}

	glm::vec3 position;

	// Falloff values determining the light's attenuation over distance
	float linear;
	float quadratic;
};

struct PSSpotLight : public PSLight
{
	PSSpotLight()
	{
		position = glm::vec3(0.0f);
		direction = glm::vec3(0.0f, 0.0f, 1.0f);
		innerDegrees = 8.75f;
		outerDegrees = 17.5f;
		cutOff = glm::cos(glm::radians(innerDegrees));
		outerCutOff = glm::cos(glm::radians(outerDegrees));
		linear = 0.014f;
		quadratic = 0.0007f;
	}

	glm::vec3 position;
	glm::vec3 direction;

	// Cosine value representing the inner cone angle for the spotlight
	float cutOff;
	// Cosine value representing the outer cone angle for the spotlight
	float outerCutOff;

	float linear;
	float quadratic;

	// Angle (in degrees) for the spotlight's inner cone where light intensity remains full
	float innerDegrees;

	// Angle (in degrees) for the spotlight's outer cone where light intensity fades from full to zero
	float outerDegrees;

	// Set the angle for the inner cone of the spotlight that maintains full intensity
	void SetInnerCutOff(float newRadius)
	{
		innerDegrees = newRadius;
		cutOff = glm::cos(glm::radians(innerDegrees));
	}

	// Set the angle for the outer cone of the spotlight where light intensity diminishes to zero
	void SetOuterCutOff(float newRadius)
	{
		outerDegrees = newRadius;
		outerCutOff = glm::cos(glm::radians(outerDegrees));
	}
};
