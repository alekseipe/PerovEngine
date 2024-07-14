// Internal headers
#include "Graphics/PModel.h"

// Vertex data for a polygon
const std::vector<PSVertexData> polyVData = {
	//   x      y      z      r     g     b       tx    ty
	{ {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} }, // Top left vertex
	{ { 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} }, // Top right vertex
	{ {-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} }, // Bottom left vertex
	{ { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }  // Bottom right vertex
};

// Index data for a polygon
const std::vector<uint32_t> polyIData = {
	0, 1, 2, // Triangle 1
	1, 2, 3  // Triangle 2
};


// Index data for a cube
const std::vector<uint32_t> cubeIData = {
	// FRONT
	0, 1, 2,
	1, 2, 3,
	// BACK
	4, 5, 6,
	5, 6, 7,
	// LEFT
	8, 9, 10,
	9, 10, 11,
	// RIGHT
	12, 13, 14,
	13, 14, 15,
	// TOP
	16, 17, 18,
	17, 18, 19,
	// BOTTOM
	20, 21, 22,
	21, 22, 23
};

// Vertex data for a cube
const std::vector<PSVertexData> cubeVData = {
	//   x      y      z      r     g     b       tx    ty
	// Front vertices
	{ {-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} }, // Top left
	{ { 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} }, // Top right
	{ {-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} }, // Bottom left
	{ { 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }, // Bottom right
	// Back vertices
	{ { 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} }, // Top left
	{ {-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} }, // Top right
	{ { 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} }, // Bottom left
	{ {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }, // Bottom right
	// Left vertices
	{ {-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} }, // Top left
	{ {-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} }, // Top right
	{ {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} }, // Bottom left
	{ {-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }, // Bottom right
	// Right vertices
	{ { 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} }, // Top left
	{ { 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} }, // Top right
	{ { 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} }, // Bottom left
	{ { 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }, // Bottom right
	// Top vertices
	{ {-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} }, // Top left
	{ { 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} }, // Top right
	{ {-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} }, // Bottom left
	{ { 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }, // Bottom right
	// Bottom vertices
	{ { 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} }, // Top left
	{ {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} }, // Top right
	{ { 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} }, // Bottom left
	{ {-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }  // Bottom right
};


void PModel::MakePoly(const TShared<PTexture>& texture)
{
	// Create a polygon mesh
	TUnique<PMesh> mesh = TMakeUnique<PMesh>();

	if (!mesh->CreateMesh(polyVData, polyIData))
	{
		PDebug::Log("Failed to create polygon mesh");
	}

	// Assign texture to the mesh and add it to the mesh stack
	mesh->SetTexture(texture);
	m_MeshStack.push_back(std::move(mesh));
}

void PModel::MakeCube(const TShared<PTexture>& texture)
{
	// Create a cube mesh
	TUnique<PMesh> mesh = TMakeUnique<PMesh>();

	if (!mesh->CreateMesh(cubeVData, cubeIData))
	{
		PDebug::Log("Failed to create cube mesh");
	}

	// Assign texture to the mesh and add it to the mesh stack
	mesh->SetTexture(texture);
	m_MeshStack.push_back(std::move(mesh));
}

void PModel::Render(const TShared<PShaderProgram>& shader)
{
	for (const auto& mesh : m_MeshStack)
	{
		mesh->Render(shader, m_Transform);
	}
}
