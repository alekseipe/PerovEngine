// Internal headers
#include "Graphics/PTexture.h"

// External libraries
#include <GLEW/glew.h>
#include <STB_IMAGE/stb_image.h>

PTexture::PTexture()
{
    m_Path = m_FileName = "";
    m_ID = 0U;
    m_Width = m_Height = m_Channels = 0;
}

PTexture::~PTexture()
{
    // Delete the texture if an ID was generated
    if (m_ID > 0)
        glDeleteTextures(1, &m_ID);

    PDebug::Log("Texture destroyed: " + m_FileName);
}

bool PTexture::LoadTexture(const PString& fileName, const PString& path)
{
    // Assign the file name and path
    m_FileName = fileName;
    m_Path = path;

    // stb_image loads images upside down, OpenGL reads them bottom-left (x:0, y:0)
    stbi_set_flip_vertically_on_load(true);

    // Load the image
    unsigned char* data = stbi_load(
        m_Path.c_str(), // Path to the image
        &m_Width, &m_Height, // Width and height of the image
        &m_Channels, // Number of channels in the image (RGBA)
        0 // We don't require a specific number of channels
    );

    if (data == nullptr)
    {
        PString error = "Failed to load texture - " + m_FileName + ": " + stbi_failure_reason();
        PDebug::Log(error, LT_ERROR);
        return false;
    }

    if (m_Channels > 4 || m_Channels < 3)
    {
        PDebug::Log("Failed to import texture - " + m_FileName + ": Incorrect number of channels, must have 3 or 4 channels");
        stbi_image_free(data);
        return false;
    }

    // Generate the texture ID in OpenGL
    glGenTextures(1, &m_ID);

    if (m_ID == 0)
    {
        PString error = reinterpret_cast<const char*>(glewGetErrorString(glGetError()));
        PString errorMsg = "Failed to generate texture ID - " + m_FileName + ": " + error;
        PDebug::Log(errorMsg, LT_ERROR);
        stbi_image_free(data);
        return false;
    }

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, m_ID);

    // Set texture parameters
    // Repeat the texture if it doesn't fit the model
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine internal format based on the number of channels
    GLint intFormat = (m_Channels == 4) ? GL_RGBA : GL_RGB;

    // Load image data into the texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        intFormat,
        m_Width, m_Height,
        0,
        intFormat,
        GL_UNSIGNED_BYTE,
        data
    );

    // Generate mipmaps for the texture
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    Unbind();

    // Free the image data
    stbi_image_free(data);

    PDebug::Log("Successfully imported texture - " + m_FileName, LT_SUCCESS);

    return true;
}

void PTexture::BindTexture(const PUi32& textureNumber)
{
    // Make the texture active for the shader in the specified slot
    glActiveTexture(GL_TEXTURE0 + textureNumber);

    glBindTexture(GL_TEXTURE_2D, m_ID);
}

void PTexture::Unbind()
{
    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}
