#pragma once
#include "EngineTypes.h"

// External Headers
#include <GLM/mat4x4.hpp>

class PShaderProgram;
struct PSTransform;
struct PSLight;
struct PSMaterial;

struct PSVertexData
{
    float m_Position[3] = { 0.0f, 0.0f, 0.0f }; // Position of the vertex
	float m_Colour[3] = { 1.0f, 1.0f, 1.0f };   // Colour of the vertex
	float m_TexCoords[2] = { 0.0f, 0.0f };      // Texture coordinates
	float m_Normal[3] = { 0.0f, 0.0f, 0.0f };   // Normal vector for lighting
    float m_Tangent[3] = { 0.0f, 0.0f, 0.0f }; // Tangent vector: x, y, z
};

class PMesh
{
public:
	PMesh();
	~PMesh();

	bool CreateMesh(const std::vector<PSVertexData>& vertices, const std::vector<uint32_t>& indices);

	void Render(const std::shared_ptr<PShaderProgram>& shader, const PSTransform& transform,
		const TArray<TShared<PSLight>>& lights, const TShared<PSMaterial>& material);

	void SetRelativeTransform(const glm::mat4& transform) { m_MatTransform = transform; }

	// Index for the material relative to the model's material array
	unsigned int materialIndex;

private:
	// Store the vertices of the mesh
	std::vector<PSVertexData> m_Vertices;

	// Store the indices for the mesh
	std::vector<uint32_t> m_Indices;

	// ID for the Vertex Array Object
	uint32_t m_VAO;

	// ID for the Vertex Buffer Object
	uint32_t m_VBO;

	// ID for the Element Array Object
	uint32_t m_EAO;

	// Mesh's transformation matrix relative to the model
	glm::mat4 m_MatTransform;
};
