#include "Graphics/PMesh.h"
#include "Debug/PDebug.h"
#include "Graphics/PShaderProgram.h"

// External Headers
#include <GLEW/glew.h>

PMesh::PMesh() {
	m_VAO = m_VBO = m_EAO = 0;
	m_MatTransform = glm::mat4(1.0f);
	materialIndex = 0;
}

PMesh::~PMesh() {}

bool PMesh::CreateMesh(const std::vector<PSVertexData>& vertices, const std::vector<uint32_t>& indices) {
	// Store the vertex data
	m_Vertices = vertices;
	m_Indices = indices;

	// Create and bind a vertex array object (VAO)
	glGenVertexArrays(1, &m_VAO);
	if (m_VAO == 0) {
		std::string errorMsg = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
		PDebug::Log("Mesh failed to create VAO: " + errorMsg, LT_WARN);
		return false;
	}
	glBindVertexArray(m_VAO);

	// Create and bind a vertex buffer object (VBO)
	glGenBuffers(1, &m_VBO);
	if (m_VBO == 0) {
		std::string errorMsg = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
		PDebug::Log("Mesh failed to create VBO: " + errorMsg, LT_WARN);
		return false;
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Create and bind an element array buffer (EAO)
	glGenBuffers(1, &m_EAO);
	if (m_EAO == 0) {
		std::string errorMsg = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
		PDebug::Log("Mesh failed to create EAO: " + errorMsg, LT_WARN);
		return false;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EAO);

	// Set buffer data for vertices and indices
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_Vertices.size() * sizeof(PSVertexData)), m_Vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_Indices.size()) * sizeof(uint32_t), m_Indices.data(), GL_STATIC_DRAW);

	// Define vertex attributes: POSITION
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PSVertexData), nullptr);

	// Define vertex attributes: COLOR
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PSVertexData), (void*)(sizeof(float) * 3));

	// Define vertex attributes: TEXTURE COORDINATES
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PSVertexData), (void*)(sizeof(float) * 6));

	// Define vertex attributes: NORMALS
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(PSVertexData), (void*)(sizeof(float) * 8));

	// Define vertex attributes: TANGENTS
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(PSVertexData), (void*)(sizeof(float) * 11));

	// Unbind the VAO
	glBindVertexArray(0);

	return true;
}

void PMesh::Render(const std::shared_ptr<PShaderProgram>& shader, const PSTransform& transform, const TArray<TShared<PSLight>>& lights, const TShared<PSMaterial>& material) {
	// Set material, transform, and lights in the shader
	shader->SetMaterial(material);
	shader->SetModelTransform(transform);
	shader->SetMeshTransform(m_MatTransform);
	shader->SetLights(lights);

	// Bind and render the VAO
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_Indices.size()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
