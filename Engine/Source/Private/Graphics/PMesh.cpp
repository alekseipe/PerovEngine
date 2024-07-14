// Internal Headers
#include "Graphics/PMesh.h"
#include "Debug/PDebug.h"
#include "Graphics/PShaderProgram.h"

// External Headers
#include <GLEW/glew.h>

PMesh::PMesh()
{
	m_VAO = m_VBO = m_EAO = 0;
	PDebug::Log("Mesh created");
}

PMesh::~PMesh()
{
	PDebug::Log("Mesh destroyed");
}

bool PMesh::CreateMesh(const std::vector<PSVertexData>& vertices, const std::vector<uint32_t>& indices)
{

	// Store vertex and index data
	m_Vertices = vertices;
	m_Indices = indices;

	// Create a Vertex Array Object (VAO)
	glGenVertexArrays(1, &m_VAO);

	if (m_VAO == 0)
	{
		std::string errorMsg = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
		PDebug::Log("Failed to create VAO: " + errorMsg, LT_WARN);
		return false;
	}

	// Bind the VAO
	glBindVertexArray(m_VAO);

	// Create and bind a Vertex Buffer Object (VBO)
	glGenBuffers(1, &m_VBO);
	if (m_VBO == 0)
	{
		std::string errorMsg = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
		PDebug::Log("Failed to create VBO: " + errorMsg, LT_WARN);
		return false;
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Create and bind an Element Array Buffer (EAO)
	glGenBuffers(1, &m_EAO);
	if (m_EAO == 0)
	{
		std::string errorMsg = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
		PDebug::Log("Failed to create EAO: " + errorMsg, LT_WARN);
		return false;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EAO);

	// Set buffer data for VBO
	glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(m_Vertices.size() * sizeof(PSVertexData)),
		m_Vertices.data(),
		GL_STATIC_DRAW
	);

	// Set buffer data for EAO
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(m_Indices.size()) * sizeof(uint32_t),
		m_Indices.data(),
		GL_STATIC_DRAW
	);

	// Define vertex attributes
	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(PSVertexData),
		nullptr
	);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(PSVertexData),
		(void*)(sizeof(float) * 3)
	);

	// Texture coordinates attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(PSVertexData),
		(void*)(sizeof(float) * 6)
	);

	// Unbind the VAO
	glBindVertexArray(0);

	return true;
}

void PMesh::Render(const std::shared_ptr<PShaderProgram>& shader, const PSTransform& transform)
{
	if (m_Texture)
	{
		shader->RunTexture(m_Texture, 0);
	}

	// Update shader with model transform
	shader->SetModelTransform(transform);

	// Bind VAO and draw elements
	glBindVertexArray(m_VAO);
	glDrawElements(
		GL_TRIANGLES,
		static_cast<GLsizei>(m_Indices.size()),
		GL_UNSIGNED_INT,
		nullptr
	);

	// Unbind the VAO
	glBindVertexArray(0);
}
