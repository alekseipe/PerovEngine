#pragma once
#include "EngineTypes.h"
#include "Graphics/PMesh.h"
#include "Math/PSTransform.h"

class PTexture;
class PShaderProgram;

// Class for managing a 3D model composed of multiple meshes
class PModel
{
public:
	PModel() = default;
	~PModel() = default;

	// Create a polygon model and add a texture to it
	void MakePoly(const TShared<PTexture>& texture);

	// Create a cube model and add a texture to it
	void MakeCube(const TShared<PTexture>& texture);

	// Render all the meshes within the model
	void Render(const TShared<PShaderProgram>& shader);

	// Get the transform of the model
	PSTransform& GetTransform() { return m_Transform; }

private:
	// Array of meshes
	TArray<TUnique<PMesh>> m_MeshStack;

	// Transform for the model in 3D space
	PSTransform m_Transform;
};
