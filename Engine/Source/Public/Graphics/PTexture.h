#pragma once
#include "EngineTypes.h"

class PTexture
{
public:
	PTexture();
	~PTexture();

	// Load a file and convert it into a texture
	bool LoadTexture(const PString& fileName, const PString& path);

	// Activate the texture for use in OpenGL
	void BindTexture(const PUi32& textureNumber);

	// Deactivate the texture in OpenGL
	void Unbind();

	// Retrieve the file path from which the texture was imported
	PString GetImportPath() const { return m_Path; }

	// Retrieve the custom file name of the texture
	PString GetName() const { return m_FileName; }

	// Retrieve the OpenGL ID associated with the texture
	PUi32 GetID() const { return m_ID; }

private:
	// File path where the texture image is stored
	PString m_Path;

	// Custom name assigned to the texture
	PString m_FileName;

	// OpenGL ID assigned to the texture
	PUi32 m_ID;

	// Texture properties: width, height, and the number of channels
	int m_Width, m_Height, m_Channels;
};
