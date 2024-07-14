#pragma once
#include "EngineTypes.h"

typedef void* SDL_GLContext;
struct SDL_Window;
class PShaderProgram;
struct PSCamera;

class PGraphicsEngine
{
public:
	PGraphicsEngine() = default;
	~PGraphicsEngine() = default;

	// Initialize the graphics engine with an SDL window and vsync option
	bool InitEngine(SDL_Window* sdlWindow, const bool& vsync);

	// Render the current frame
	void Render(SDL_Window* sdlWindow);

	// Get a weak pointer to the camera
	TWeak<PSCamera> GetCamera() { return m_Camera; }

private:
	// OpenGL context for the SDL window
	SDL_GLContext m_SDLGLContext;

	// Shader program used by the engine
	TShared<PShaderProgram> m_Shader;

	// Camera used by the engine
	TShared<PSCamera> m_Camera;
};
