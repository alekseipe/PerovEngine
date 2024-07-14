#pragma once
#include "EngineTypes.h"

class PTexture
{
public:
	PTexture();
	~PTexture();

	// Load an image file and convert it to a texture
	bool LoadTexture(const PString& fileName, const PString& path);

	// Bind the texture for use in OpenGL
	void BindTexture(const PUi32& textureNumber);

	// Unbind the texture in OpenGL
	void Unbind();

	// Get the import path of the texture
	PString GetImportPath() const { return m_Path; }

	// Get the custom file name of the texture
	PString GetName() const { return m_FileName; }

	// Get the OpenGL ID of the texture
	PUi32 GetID() const { return m_ID; }

private:
	// Import path of the image
	PString m_Path;

	// Custom name of the texture
	PString m_FileName;

	// OpenGL ID for the texture
	PUi32 m_ID;

	// Texture parameters: width, height, and number of channels
	int m_Width, m_Height, m_Channels;
};
