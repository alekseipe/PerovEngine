#pragma once
#include "EngineTypes.h"
#include "Graphics/PMesh.h"
#include "Math/PSTransform.h"

// External Headers
#include <ASSIMP/matrix4x4.h>

class PTexture;
class PShaderProgram;
struct aiScene;
struct aiNode;
struct PSLight;
struct PSMaterial;

class PModel
{
public:
	PModel() { }
	~PModel() = default;

	// Import a 3D model from a file
	// Utilizes the ASSIMP library; refer to documentation for supported file types
	void ImportModel(const PString& filePath);

	// Render all meshes within the model
	// The transformation of meshes is based on the model's transformation
	void Render(const TShared<PShaderProgram>& shader, const TArray<TShared<PSLight>>& lights);

	// Retrieve the model's transformation
	PSTransform& GetTransform() { return m_Transform; }

	// Assign a material to a specific slot number
	void SetMaterialBySlot(unsigned int slot, const TShared<PSMaterial>& material);

private:
	// Array of mesh objects within the model
	TArray<TUnique<PMesh>> m_MeshStack;

	// Transformation data for the model in 3D space
	PSTransform m_Transform;

	// Array of materials associated with the model
	TArray<TShared<PSMaterial>> m_MaterialsStack;

	// Locate and import all meshes from a scene, converting them to PMesh
	bool FindAndImportMeshes(const aiNode& node, const aiScene& scene,
		const aiMatrix4x4& parentTransform, PUi32* meshesCreated);
};
