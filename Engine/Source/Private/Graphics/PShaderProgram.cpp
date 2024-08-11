#include "Graphics/PShaderProgram.h"
#include "Debug/PDebug.h"
#include "Math/PSTransform.h"
#include "Graphics/PTexture.h"
#include "Graphics/PSCamera.h"
#include "Graphics/PSLight.h"
#include "Graphics/PSMaterial.h"

// External Headers
#include <GLEW/glew.h>
#include <GLM/gtc/type_ptr.hpp>

// System Headers
#include <fstream>
#include <sstream>

#define LGET_GLEW_ERROR reinterpret_cast<const char*>(glewGetErrorString(glGetError()));

// Constant values for the maximum number of lights
const PUi32 maxDirLights = 1;
const PUi32 maxPointLights = 20;
const PUi32 maxSpotLights = 20;

PShaderProgram::PShaderProgram() {
	m_ProgramID = 0;
}

PShaderProgram::~PShaderProgram() {
	PDebug::Log("Shader prog " + std::to_string(m_ProgramID) + " destroyed");
}

bool PShaderProgram::InitShader(const PString& vShaderPath, const PString& fShaderPath) {
	// Create the shader program in OpenGL
	m_ProgramID = glCreateProgram();

	// Check if the program creation failed
	if (m_ProgramID == 0) {
		const std::string errorMessage = LGET_GLEW_ERROR;
		PDebug::Log("Shader init failed, couldn't create program: " + errorMessage);
		return false;
	}

	// Import shaders and check for failures
	if (!ImportShaderByType(vShaderPath, ST_VERTEX) || !ImportShaderByType(fShaderPath, ST_FRAGMENT)) {
		PDebug::Log("Shader prog failed to init, couldnt import shaders");
		return false;
	}

	return LinkToGPU();
}

void PShaderProgram::Activate() {
	glUseProgram(m_ProgramID);
}

void PShaderProgram::SetMeshTransform(const glm::mat4& matTransform) {
	// Find the variable in the shader
	const int varID = glGetUniformLocation(m_ProgramID, "mesh");
	// Update the value
	glUniformMatrix4fv(varID, 1, GL_FALSE, value_ptr(matTransform));
}

void PShaderProgram::SetModelTransform(const PSTransform& transform) {
	// Initialize a default matrix transform
	glm::mat4 matrixT = glm::mat4(1.0f);

	// Apply translation
	matrixT = glm::translate(matrixT, transform.position);

	// Apply rotations on each axis
	matrixT = glm::rotate(matrixT, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	matrixT = glm::rotate(matrixT, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	matrixT = glm::rotate(matrixT, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Apply scaling
	matrixT = glm::scale(matrixT, transform.scale);

	// Find the variable in the shader
	const int varID = glGetUniformLocation(m_ProgramID, "model");

	// Update the value
	glUniformMatrix4fv(varID, 1, GL_FALSE, value_ptr(matrixT));
}

void PShaderProgram::SetWorldTransform(const TShared<PSCamera>& camera) {
	// Initialize a matrix
	glm::mat4 matrixT = glm::mat4(1.0f);

	// Handle the view matrix (camera transformation)
	matrixT = glm::lookAt(
		camera->transform.position,
		camera->transform.position + camera->transform.Forward(),
		camera->transform.Up()
	);

	// Find the variable in the shader and update it
	int varID = glGetUniformLocation(m_ProgramID, "view");
	glUniformMatrix4fv(varID, 1, GL_FALSE, glm::value_ptr(matrixT));

	// Handle the projection matrix
	matrixT = glm::perspective(glm::radians(camera->fov), camera->aspectRatio, camera->nearClip, camera->farClip);

	// Find the variable in the shader for the projection matrix
	varID = glGetUniformLocation(m_ProgramID, "projection");
	glUniformMatrix4fv(varID, 1, GL_FALSE, value_ptr(matrixT));
}

void PShaderProgram::SetLights(const TArray<TShared<PSLight>>& lights) {
	PUi32 dirLights = 0, pointLights = 0, spotLights = 0;
	int varID = 0;
	PString lightIndexStr = "";

	// Loop through all lights and add them to the shader
	for (PUi32 i = 0; i < lights.size(); ++i) {
		if (const TShared<PSDirLight>& lightRef = std::dynamic_pointer_cast<PSDirLight>(lights[i])) {
			if (dirLights >= maxDirLights) continue;

			lightIndexStr = "dirLights[" + std::to_string(dirLights) + "]";

			// Set light properties in the shader
			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".colour").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->colour));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".ambient").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->ambient));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".direction").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->direction));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".intensity").c_str());
			glUniform1f(varID, lightRef->intensity);

			++dirLights;
			continue;
		}

		if (const TShared<PSPointLight>& lightRef = std::dynamic_pointer_cast<PSPointLight>(lights[i])) {
			if (pointLights >= maxPointLights) continue;

			lightIndexStr = "pointLights[" + std::to_string(pointLights) + "]";

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".colour").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->colour));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".position").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->position));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".intensity").c_str());
			glUniform1f(varID, lightRef->intensity);

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".linear").c_str());
			glUniform1f(varID, lightRef->linear);

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".quadratic").c_str());
			glUniform1f(varID, lightRef->quadratic);

			++pointLights;
		}

		if (const TShared<PSSpotLight>& lightRef = std::dynamic_pointer_cast<PSSpotLight>(lights[i])) {
			if (spotLights >= maxSpotLights) continue;

			lightIndexStr = "spotLights[" + std::to_string(spotLights) + "]";

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".colour").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->colour));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".position").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->position));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".direction").c_str());
			glUniform3fv(varID, 1, glm::value_ptr(lightRef->direction));

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".cutOff").c_str());
			glUniform1f(varID, lightRef->cutOff);

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".outerCutOff").c_str());
			glUniform1f(varID, lightRef->outerCutOff);

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".intensity").c_str());
			glUniform1f(varID, lightRef->intensity);

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".linear").c_str());
			glUniform1f(varID, lightRef->linear);

			varID = glGetUniformLocation(m_ProgramID, (lightIndexStr + ".quadratic").c_str());
			glUniform1f(varID, lightRef->quadratic);

			++spotLights;
		}
	}
}

void PShaderProgram::SetMaterial(const TShared<PSMaterial>& material) {
	if (!material) return;

	int varID = 0;

	// Set base color (diffuse) map
	if (material->m_BaseColourMap) {
		material->m_BaseColourMap->BindTexture(0);
		varID = glGetUniformLocation(m_ProgramID, "material.baseColourMap");
		glUniform1i(varID, 0);
	}

	// Set specular map
	if (material->m_SpecularMap) {
		material->m_SpecularMap->BindTexture(1);
		varID = glGetUniformLocation(m_ProgramID, "material.specularMap");
		glUniform1f(varID, 1);
	}

	// Set normal map
	if (material->m_NormalMap) {
		material->m_NormalMap->BindTexture(2);
		varID = glGetUniformLocation(m_ProgramID, "material.normalMap");
		glUniform1f(varID, 2);
	}

	// Set shininess
	varID = glGetUniformLocation(m_ProgramID, "material.shininess");
	glUniform1f(varID, material->shininess);

	// Set specular strength
	varID = glGetUniformLocation(m_ProgramID, "material.specularStrength");
	glUniform1f(varID, material->specularStrength);
}

bool PShaderProgram::ImportShaderByType(const PString& filePath, PEShaderType shaderType) {
	// Convert the shader file to a string
	const PString shaderStr = ConvertFileToString(filePath);

	// Ensure the shader string is not empty
	if (shaderStr.empty()) {
		PDebug::Log("Shader failed to import", LT_ERROR);
		return false;
	}

	// Create an ID for the shader based on its type
	switch (shaderType) {
	case ST_VERTEX:
		m_ShaderIDs[shaderType] = glCreateShader(GL_VERTEX_SHADER);
		break;
	case ST_FRAGMENT:
		m_ShaderIDs[shaderType] = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	default:
		break;
	}

	// Ensure the shader ID is valid
	if (m_ShaderIDs[shaderType] == 0) {
		const PString errorMessage = LGET_GLEW_ERROR;
		PDebug::Log("Shader program could not assign shader ID: " + errorMessage, LT_ERROR);
		return false;
	}

	// Compile the shader
	const char* shaderCStr = shaderStr.c_str();
	glShaderSource(m_ShaderIDs[shaderType], 1, &shaderCStr, nullptr);
	glCompileShader(m_ShaderIDs[shaderType]);

	// Check if the compilation was successful
	GLint success;
	glGetShaderiv(m_ShaderIDs[shaderType], GL_COMPILE_STATUS, &success);

	if (!success) {
		// Log the error
		char infoLog[512];
		glGetShaderInfoLog(m_ShaderIDs[shaderType], 512, nullptr, infoLog);
		PDebug::Log("Shader compilation error: " + PString(infoLog), LT_ERROR);
		return false;
	}

	// Attach the shader to the program ID
	glAttachShader(m_ProgramID, m_ShaderIDs[shaderType]);

	return true;
}

PString PShaderProgram::ConvertFileToString(const PString& filePath) {
	// Open the file stream
	std::ifstream shaderSource(filePath);

	// Check if the file was opened successfully
	if (!shaderSource.is_open()) {
		PDebug::Log("Failed to open file: " + filePath, LT_ERROR);
		return "";
	}

	// Convert the file contents to a string
	std::stringstream shaderStream;
	shaderStream << shaderSource.rdbuf();

	// Close the file
	shaderSource.close();

	return shaderStream.str();
}

bool PShaderProgram::LinkToGPU() {
	// Link the program to the GPU
	glLinkProgram(m_ProgramID);

	// Check if the linking was successful
	GLint success;
	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);

	if (!success) {
		// Log the error
		char infoLog[512];
		glGetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
		PDebug::Log("Shader link error: " + PString(infoLog), LT_ERROR);
		return false;
	}

	PDebug::Log("Shader successfully initialized and linked at index: " + std::to_string(m_ProgramID));
	return true;
}
