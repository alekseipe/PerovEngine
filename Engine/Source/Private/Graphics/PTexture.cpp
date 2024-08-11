#include "Graphics/PTexture.h"

// External Headers
#include <GLEW/glew.h>
#include <STB_IMAGE/stb_image.h>

PTexture::PTexture() {
    m_Path = m_FileName = "";
    m_ID = 0U;
    m_Width = m_Height = m_Channels = 0;
}

PTexture::~PTexture() {
    // If a valid texture ID exists, delete the texture
    if (m_ID > 0)
        glDeleteTextures(1, &m_ID);

    PDebug::Log("Texture destroyed: " + m_FileName);
}

bool PTexture::LoadTexture(const PString& fileName, const PString& path) {
    // Assign the file name and path
    m_FileName = fileName;
    m_Path = path;

    // stb_image loads images upside down; OpenGL expects them flipped (origin at bottom-left)
    stbi_set_flip_vertically_on_load(true);

    // Load the image data
    unsigned char* data = stbi_load(
        m_Path.c_str(), // Image path
        &m_Width, &m_Height, // Image dimensions
        &m_Channels, // Number of color channels
        0 // Force number of channels (0 = no limit)
    );

    // Check if the image data was loaded
    if (data == nullptr) {
        PString error = "Failed to load texture - " + m_FileName + ": " + stbi_failure_reason();
        PDebug::Log(error, LT_ERROR);
        return false;
    }

    // Ensure the image has 3 or 4 channels
    if (m_Channels > 4 || m_Channels < 3) {
        PDebug::Log("Failed to import texture - " + m_FileName + ": Incorrect number of channels, must have 3 or 4 channels");
        return false;
    }

    // Generate the OpenGL texture ID
    glGenTextures(1, &m_ID);

    // Check if texture ID generation failed
    if (m_ID == 0) {
        PString error = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
        PString errorMsg = "Failed to generate texture ID - " + m_FileName + ": " + error;
        PDebug::Log(errorMsg, LT_ERROR);
    }

    // Bind the texture to OpenGL
    glBindTexture(GL_TEXTURE_2D, m_ID);

    // Set texture wrapping parameters (repeat if the texture doesn't fit the model)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // s = x-axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // t = y-axis

    // Set filtering parameters (controls blurring when texture resolution is lower than model size)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Default format is RGB (3 channels)
    GLint intFormat = GL_RGB;
    // If the image has 4 channels, set the format to RGBA
    if (m_Channels == 4)
        intFormat = GL_RGBA;

    // Load the image data into the bound texture
    glTexImage2D(
        GL_TEXTURE_2D, // Target texture type
        0, // Mipmap level (0 = base level)
        intFormat, // Internal format
        m_Width, m_Height, // Texture dimensions
        0, // Border (legacy, should be 0)
        intFormat, // External format
        GL_UNSIGNED_BYTE, // Data type
        data // Pointer to the image data
    );

    // Generate mipmaps (lower resolution versions of the texture for distant rendering)
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture to free the slot
    Unbind();

    // Free the loaded image data
    stbi_image_free(data);

    PDebug::Log("Successfully imported texture - " + m_FileName, LT_SUCCESS);
    return true;
}

void PTexture::BindTexture(const PUi32& textureNumber) {
    // Activate the texture slot and bind the texture to it
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

void PTexture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
