#include "Graphics/PModel.h"

// External Headers
#include <ASSIMP/Importer.hpp>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>
#include <ASSIMP/mesh.h>

void PModel::ImportModel(const PString& filePath) {
	// Create an Assimp importer
	Assimp::Importer importer;

	// Read the file and convert the model to an Assimp scene with triangulation
	const auto scene = importer.ReadFile(filePath, aiProcess_Triangulate);

	// Check if the import failed
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		PDebug::Log("Error importing model from " + filePath + " : " + importer.GetErrorString(), LT_ERROR);
		return;
	}

	aiMatrix4x4 sceneTransform;
	aiMatrix4x4::Translation({ 0.0f, 0.0f, 0.0f }, sceneTransform);
	sceneTransform.FromEulerAnglesXYZ({ 0.0f, 0.0f, 0.0f });
	aiMatrix4x4::Scaling({ 1.0f, 1.0f, 1.0f }, sceneTransform);

	PUi32 meshesCreated = 0;

	// Find and import all meshes in the scene
	if (!FindAndImportMeshes(*scene->mRootNode, *scene, sceneTransform, &meshesCreated)) {
		PDebug::Log("Model failed to convert ASSIMP scene: " + filePath, LT_ERROR);
		return;
	}

	m_MaterialsStack.resize(scene->mNumMaterials);
	PDebug::Log("Model successfully imported with (" + std::to_string(meshesCreated) + ") meshes: " + filePath, LT_SUCCESS);
}

void PModel::Render(const TShared<PShaderProgram>& shader, const TArray<TShared<PSLight>>& lights) {
	for (const auto& mesh : m_MeshStack) {
		mesh->Render(shader, m_Transform, lights, m_MaterialsStack[mesh->materialIndex]);
	}
}

void PModel::SetMaterialBySlot(unsigned int slot, const TShared<PSMaterial>& material) {
	if (slot >= m_MaterialsStack.size()) {
		PDebug::Log("No material slot exists at that index: " + std::to_string(slot), LT_WARN);
		return;
	}
	m_MaterialsStack[slot] = material;
}

bool PModel::FindAndImportMeshes(const aiNode& node, const aiScene& scene, const aiMatrix4x4& parentTransform, PUi32* meshesCreated) {
	for (PUi32 i = 0; i < node.mNumMeshes; ++i) {
		const auto aMesh = scene.mMeshes[node.mMeshes[i]];

		TArray<PSVertexData> meshVertices;
		TArray<PUi32> meshIndices;

		for (PUi64 j = 0; j < aMesh->mNumVertices; ++j) {
			PSVertexData vertex;
			vertex.m_Position[0] = aMesh->mVertices[j].x;
			vertex.m_Position[1] = aMesh->mVertices[j].y;
			vertex.m_Position[2] = aMesh->mVertices[j].z;

			if (aMesh->HasVertexColors(j)) {
				vertex.m_Colour[0] = aMesh->mColors[j]->r;
				vertex.m_Colour[1] = aMesh->mColors[j]->g;
				vertex.m_Colour[2] = aMesh->mColors[j]->b;
			}

			if (aMesh->HasTextureCoords(0)) {
				vertex.m_TexCoords[0] = aMesh->mTextureCoords[0][j].x;
				vertex.m_TexCoords[1] = aMesh->mTextureCoords[0][j].y;
			}

			vertex.m_Normal[0] = aMesh->mNormals[j].x;
			vertex.m_Normal[1] = aMesh->mNormals[j].y;
			vertex.m_Normal[2] = aMesh->mNormals[j].z;

			if (aMesh->HasTangentsAndBitangents()) {
				vertex.m_Tangent[0] = aMesh->mTangents[j].x;
				vertex.m_Tangent[1] = aMesh->mTangents[j].y;
				vertex.m_Tangent[2] = aMesh->mTangents[j].z;
			}

			meshVertices.push_back(vertex);
		}

		if (meshVertices.size() < 3) {
			PDebug::Log("Mesh has less than 3 vertices", LT_ERROR);
			return false;
		}

		for (PUi64 j = 0; j < aMesh->mNumFaces; ++j) {
			auto face = aMesh->mFaces[j];
			for (PUi32 k = 0; k < face.mNumIndices; ++k) {
				meshIndices.push_back(face.mIndices[k]);
			}
		}

		auto pMesh = TMakeUnique<PMesh>();

		if (!pMesh->CreateMesh(meshVertices, meshIndices)) {
			PDebug::Log("Mesh failed to convert from A Mesh to P Mesh", LT_ERROR);
			return false;
		}

		pMesh->materialIndex = aMesh->mMaterialIndex;

		aiMatrix4x4 relTransform = parentTransform * node.mTransformation;
		glm::mat4 matTransform(1.0f);

		matTransform[0][0] = relTransform.a1; matTransform[1][0] = relTransform.a2;
		matTransform[2][0] = relTransform.a3; matTransform[3][0] = relTransform.a4;
		matTransform[0][1] = relTransform.b1; matTransform[1][1] = relTransform.b2;
		matTransform[2][1] = relTransform.b3; matTransform[3][1] = relTransform.b4;
		matTransform[0][2] = relTransform.c1; matTransform[1][2] = relTransform.c2;
		matTransform[2][2] = relTransform.c3; matTransform[3][2] = relTransform.c4;
		matTransform[0][3] = relTransform.d1; matTransform[1][3] = relTransform.d2;
		matTransform[2][3] = relTransform.d3; matTransform[3][3] = relTransform.d4;

		pMesh->SetRelativeTransform(matTransform);
		m_MeshStack.push_back(std::move(pMesh));
		++*meshesCreated;
	}

	const aiMatrix4x4 nodeRelTransform = parentTransform * node.mTransformation;

	for (PUi32 i = 0; i < node.mNumChildren; ++i) {
		if (!FindAndImportMeshes(*node.mChildren[i], scene, nodeRelTransform, meshesCreated)) {
			return false;
		}
	}

	return true;
}
