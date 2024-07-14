#pragma once
#include "EngineTypes.h"

class PShaderProgram;
struct PSTransform;
class PTexture;

// Structure for storing vertex data
struct PSVertexData
{
	float m_Position[3] = { 0.0f, 0.0f, 0.0f }; // Position of the vertex
	float m_Colour[3] = { 1.0f, 1.0f, 1.0f };   // Colour of the vertex
	float m_TexCoords[2] = { 0.0f, 0.0f };      // Texture coordinates
	float m_Normal[3] = { 0.0f, 0.0f, 0.0f };   // Normal vector for lighting
};

// Class for managing a mesh
class PMesh
{
public:
	PMesh();
	~PMesh();

	// Create a mesh using vertex and index data
	bool CreateMesh(const std::vector<PSVertexData>& vertices, const std::vector<uint32_t>& indices);

	// Render the mesh with a given shader and transform
	void Render(const std::shared_ptr<PShaderProgram>& shader, const PSTransform& transform);

	// Set the texture for the mesh
	void SetTexture(const TShared<PTexture>& texture) { m_Texture = texture; }

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

	// Texture for the mesh
	TShared<PTexture> m_Texture;
};
