// Internal headers
#include "Graphics/PShaderProgram.h"
#include "Debug/PDebug.h"
#include "Math/PSTransform.h"
#include "Graphics/PTexture.h"
#include "Graphics/PSCamera.h"

// External libraries
#include <GLEW/glew.h>
#include <GLM/gtc/type_ptr.hpp>

// System libraries
#include <fstream>
#include <sstream>

// Macro for getting GLEW error string
#define LGET_GLEW_ERROR reinterpret_cast<const char*>(glewGetErrorString(glGetError()));

PShaderProgram::PShaderProgram()
{
	m_ProgramID = 0;
}

PShaderProgram::~PShaderProgram()
{
	PDebug::Log("Shader program " + std::to_string(m_ProgramID) + " destroyed");
}

bool PShaderProgram::InitShader(const PString& vShaderPath, const PString& fShaderPath)
{
	// Create the shader program
	m_ProgramID = glCreateProgram();

	if (m_ProgramID == 0)
	{
		const std::string errorMessage = LGET_GLEW_ERROR;
		PDebug::Log("Failed to initialize shader, couldn't create program: " + errorMessage);
		return false;
	}

	// Import and compile vertex and fragment shaders
	if (!ImportShaderByType(vShaderPath, ST_VERTEX) || !ImportShaderByType(fShaderPath, ST_FRAGMENT))
	{
		PDebug::Log("Failed to initialize shader program, couldn't import shaders");
		return false;
	}

	return LinkToGPU();
}

void PShaderProgram::Activate()
{
	glUseProgram(m_ProgramID);
}

void PShaderProgram::SetModelTransform(const PSTransform& transform)
{
	// Initialize a default matrix transform
	glm::mat4 matrixT = glm::mat4(1.0f);

	// Apply transformations: translate, rotate, then scale
	matrixT = glm::translate(matrixT, transform.position);
	matrixT = glm::rotate(matrixT, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	matrixT = glm::rotate(matrixT, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	matrixT = glm::rotate(matrixT, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	matrixT = glm::scale(matrixT, transform.scale);

	// Get the location of the "model" uniform variable in the shader
	const int varID = glGetUniformLocation(m_ProgramID, "model");

	// Update the "model" uniform with the transformation matrix
	glUniformMatrix4fv(varID, 1, GL_FALSE, glm::value_ptr(matrixT));
}

void PShaderProgram::SetWorldTransform(const TShared<PSCamera>& camera)
{
	// Initialize view matrix
	glm::mat4 viewMatrix = glm::lookAt(
		camera->transform.position,
		camera->transform.position + camera->transform.Forward(),
		camera->transform.Up()
	);

	// Get the location of the "view" uniform variable in the shader and update it
	int viewVarID = glGetUniformLocation(m_ProgramID, "view");
	glUniformMatrix4fv(viewVarID, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Initialize projection matrix
	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(camera->fov),
		camera->aspectRatio,
		camera->nearClip,
		camera->farClip
	);

	// Get the location of the "projection" uniform variable in the shader and update it
	int projectionVarID = glGetUniformLocation(m_ProgramID, "projection");
	glUniformMatrix4fv(projectionVarID, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void PShaderProgram::RunTexture(const TShared<PTexture>& texture, const PUi32& slot)
{
	// Bind the texture
	texture->BindTexture(slot);

	// Get the uniform variable location for the texture slot
	int varID = 0;
	if (slot == 0)
	{
		varID = glGetUniformLocation(m_ProgramID, "colourMap");
	}

	// Update the shader with the texture slot
	glUniform1i(varID, slot);
}

bool PShaderProgram::ImportShaderByType(const PString& filePath, PEShaderType shaderType)
{
	// Convert the shader file to a string
	const PString shaderStr = ConvertFileToString(filePath);

	if (shaderStr.empty())
	{
		PDebug::Log("Failed to import shader", LT_ERROR);
		return false;
	}

	// Create the shader based on its type
	switch (shaderType)
	{
	case ST_VERTEX:
		m_ShaderIDs[shaderType] = glCreateShader(GL_VERTEX_SHADER);
		break;
	case ST_FRAGMENT:
		m_ShaderIDs[shaderType] = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	default:
		break;
	}

	if (m_ShaderIDs[shaderType] == 0)
	{
		const PString errorMessage = LGET_GLEW_ERROR;
		PDebug::Log("Failed to assign shader ID: " + errorMessage, LT_ERROR);
		return false;
	}

	// Compile the shader
	const char* shaderCStr = shaderStr.c_str();
	glShaderSource(m_ShaderIDs[shaderType], 1, &shaderCStr, nullptr);
	glCompileShader(m_ShaderIDs[shaderType]);

	// Check if the compilation was successful
	GLint success;
	glGetShaderiv(m_ShaderIDs[shaderType], GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(m_ShaderIDs[shaderType], 512, nullptr, infoLog);
		PDebug::Log("Shader compilation error: " + PString(infoLog), LT_ERROR);
		return false;
	}

	// Attach the shader to the program
	glAttachShader(m_ProgramID, m_ShaderIDs[shaderType]);

	return true;
}

PString PShaderProgram::ConvertFileToString(const PString& filePath)
{
	// Open the file
	std::ifstream shaderSource(filePath);

	if (!shaderSource.is_open())
	{
		PDebug::Log("Failed to open file: " + filePath, LT_ERROR);
		return "";
	}

	// Convert the file contents to a string
	std::stringstream shaderStream;
	shaderStream << shaderSource.rdbuf();
	shaderSource.close();

	return shaderStream.str();
}

bool PShaderProgram::LinkToGPU()
{
	// Link the shader program
	glLinkProgram(m_ProgramID);

	// Check if the linking was successful
	GLint success;
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(m_ProgramID, 512, nullptr, infoLog);
		PDebug::Log("Shader link error: " + PString(infoLog), LT_ERROR);
		return false;
	}

	PDebug::Log("Shader successfully initialized and linked with ID: " + std::to_string(m_ProgramID));

	return true;
}
