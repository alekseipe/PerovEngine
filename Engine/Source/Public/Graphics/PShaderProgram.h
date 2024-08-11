#pragma once
#include "EngineTypes.h"

// External Headers
#include <GLM/mat4x4.hpp>

class PTexture;
struct PSCamera;
struct PSMaterial;

// Enum to specify the type of shader
enum PEShaderType : PUi8
{
	ST_VERTEX = 0U,
	ST_FRAGMENT
};

struct PSTransform;
struct PSLight;

class PShaderProgram
{
public:
	PShaderProgram();
	~PShaderProgram();

	// Initialize the shader program using vertex and fragment shader files
	bool InitShader(const PString& vShaderPath, const PString& fShaderPath);

	// Activate the shader program for use
	// Modifications to shader values require the shader to be active
	void Activate();

	// Set the mesh transformation matrix in the shader
	void SetMeshTransform(const glm::mat4& matTransform);

	// Set the model's transformation properties in the shader
	void SetModelTransform(const PSTransform& transform);

	// Set the 3D world transformation matrix using the camera's view in the shader
	void SetWorldTransform(const TShared<PSCamera>& camera);

	// Set the lighting properties in the shader
	void SetLights(const TArray<TShared<PSLight>>& lights);

	// Set the material properties in the shader
	void SetMaterial(const TShared<PSMaterial>& material);

private:
	// File paths for the vertex and fragment shaders
	PString m_FilePath[2] = { "", "" };

	// Shader IDs for the vertex and fragment shaders
	PUi32 m_ShaderIDs[2] = { 0, 0 };

	// ID for the shader program
	PUi32 m_ProgramID;

	// Import a shader from a file based on the specified shader type
	bool ImportShaderByType(const PString& filePath, PEShaderType shaderType);

	// Convert the contents of a file into a string
	PString ConvertFileToString(const PString& filePath);

	// Link the shader program to the GPU using OpenGL
	bool LinkToGPU();
};
