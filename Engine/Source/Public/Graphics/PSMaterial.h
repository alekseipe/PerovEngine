#pragma once
#include "EngineTypes.h"

class PTexture;

struct PSMaterial
{
	PSMaterial() = default;

	// Texture that defines the base color of the material
	TShared<PTexture> m_BaseColourMap;

	// Texture that represents the shininess of the material
	// Typically a grayscale map
	// White areas are fully shiny (reflective, indicating strong reflection of light)
	TShared<PTexture> m_SpecularMap;

	// Texture that simulates the surface depth and bumpiness, giving a more realistic 3D appearance
	TShared<PTexture> m_NormalMap;

	// Material properties
	// Defines the shininess level of the material
	float shininess = 32.0f;

	// Controls the intensity of the specular highlights
	float specularStrength = 0.5f;
};
