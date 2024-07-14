// Internal headers
#include "Graphics/PGraphicsEngine.h"
#include "Graphics/PModel.h"
#include "Graphics/PShaderProgram.h"
#include "Math/PSTransform.h"
#include "Graphics/PTexture.h"
#include "Graphics/PSCamera.h"

// External headers
#include <GLEW/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

// Test mesh for debugging
TUnique<PModel> m_Model;

bool PGraphicsEngine::InitEngine(SDL_Window* sdlWindow, const bool& vsync)
{

	if (sdlWindow == nullptr)
	{
		PDebug::Log("SDL window is null", LT_ERROR);
		PDebug::Log("Graphics engine initialization failed", LT_ERROR);
		return false;
	}

	// Create an OpenGL context
	m_SDLGLContext = SDL_GL_CreateContext(sdlWindow);

	if (m_SDLGLContext == nullptr)
	{
		PDebug::Log("Failed to create SDL GL context: " + std::string(SDL_GetError()), LT_ERROR);
		PDebug::Log("Graphics engine initialization failed", LT_ERROR);
		return false;
	}

	// Make the current context active
	if (SDL_GL_MakeCurrent(sdlWindow, m_SDLGLContext) != 0)
	{
		PDebug::Log("Failed to make GL context current: " + std::string(SDL_GetError()), LT_ERROR);
		PDebug::Log("Graphics engine initialization failed", LT_ERROR);
		return false;
	}

	if (vsync)
	{
		// Enable adaptive vsync, fallback to standard vsync if it fails
		if (SDL_GL_SetSwapInterval(-1) != 0 && SDL_GL_SetSwapInterval(1) != 0)
		{
			PDebug::Log("Vsync initialization failed: " + std::string(SDL_GetError()), LT_WARN);
			return false;
		}
	}

	// Initialize GLEW
	GLenum glewResult = glewInit();
	if (glewResult != GLEW_OK)
	{
		PDebug::Log("GLEW initialization failed: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(glewResult))), LT_ERROR);
		return false;
	}

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Create and initialize the shader
	m_Shader = TMakeShared<PShaderProgram>();
	if (!m_Shader->InitShader("Shaders/SimpleShader/SimpleShader.vertex", "Shaders/SimpleShader/SimpleShader.frag"))
	{
		PDebug::Log("Shader initialization failed", LT_ERROR);
		return false;
	}

	// Create the camera
	m_Camera = TMakeShared<PSCamera>();
	m_Camera->transform.position.z = -5.0f;

	// Create and load the default texture
	TShared<PTexture> defaultTexture = TMakeShared<PTexture>();
	if (!defaultTexture->LoadTexture("Default Grid", "Textures/DefaultGrid.png"))
	{
		PDebug::Log("Default texture loading failed", LT_ERROR);
	}

	// DEBUG: Create and initialize a test model
	m_Model = TMakeUnique<PModel>();
	m_Model->MakeCube(defaultTexture);

	// Log successful initialization
	PDebug::Log("Graphics engine initialized successfully", LT_SUCCESS);

	return true;
}

void PGraphicsEngine::Render(SDL_Window* sdlWindow)
{
	// Set background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Clear the back buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate the shader
	m_Shader->Activate();

	// Set world transformations based on the camera
	m_Shader->SetWorldTransform(m_Camera);

	// Render the model
	m_Model->Render(m_Shader);

	// Swap the back buffer with the front buffer to present the frame
	SDL_GL_SwapWindow(sdlWindow);
}
