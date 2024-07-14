#pragma once
#include "EngineTypes.h"

class PTexture;
struct PSCamera;

// Enum to determine the type of shader
enum PEShaderType : PUi8
{
	ST_VERTEX = 0U,  // Vertex shader
	ST_FRAGMENT      // Fragment shader
};

struct PSTransform;

class PShaderProgram
{
public:
	PShaderProgram();
	~PShaderProgram();

	// Initialize the shader program using vertex and fragment shader files
	bool InitShader(const PString& vShaderPath, const PString& fShaderPath);

	// Activate the shader program for use
	void Activate();

	// Set the model transformation matrix in the shader
	void SetModelTransform(const PSTransform& transform);

	// Set the world transformation matrices (view and projection) in the shader
	void SetWorldTransform(const TShared<PSCamera>& camera);

	// Bind a texture to a specific slot in the shader
	void RunTexture(const TShared<PTexture>& texture, const PUi32& slot);

private:
	// Store the file paths for the vertex and fragment shaders
	PString m_FilePath[2] = { "", "" };

	// Store the shader IDs for the vertex and fragment shaders
	PUi32 m_ShaderIDs[2] = { 0, 0 };

	// Store the ID for the shader program
	PUi32 m_ProgramID;

	// Import a shader from a file based on its type (vertex or fragment)
	bool ImportShaderByType(const PString& filePath, PEShaderType shaderType);

	// Convert the contents of a file to a string
	PString ConvertFileToString(const PString& filePath);

	// Link the shader program to the GPU
	bool LinkToGPU();
};
