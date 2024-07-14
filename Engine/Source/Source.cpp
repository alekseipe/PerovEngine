#include "EngineTypes.h"

// External libraries
#include <SDL/SDL.h>

// Engine libraries
#include "PWindow.h"
#include "Listeners/PInput.h"
#include "Graphics/PSCamera.h"

// Note on smart pointers:
// - Shared pointer: Shares ownership across all references.
// - Unique pointer: Does not share ownership with anything.
// - Weak pointer: Has no ownership over any references.

// Global variables
TShared<PWindow> m_Window = nullptr;
TShared<PInput> m_Input = nullptr;

// Initialize SDL and create the window and input system
bool Initialise()
{
	// Initialize the required SDL components
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
	{
		std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	// Set SDL to use OpenGL version 4.6
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	// Ensure compatibility mode if OpenGL version is not supported by SDL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	// Set color bit depth
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);

	// Create the window
	m_Window = TMakeShared<PWindow>();
	if (!m_Window->CreateWindow({ "Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 720 }))
	{
		return false;
	}

	// Create the input class and initialize it with the window
	m_Input = TMakeShared<PInput>();
	m_Input->InitInput(m_Window);

	return true;
}

// Clean up and shut down SDL
void Cleanup()
{
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	// Initialize the engine
	if (!Initialise())
	{
		Cleanup();
		return -1;
	}

	// Register input handling for the window
	m_Window->RegisterInput(m_Input);

	// Main game loop: run until the window is closed
	while (!m_Window->IsPendingClose())
	{
		// Update input states
		m_Input->UpdateInputs();

		// Render the scene
		m_Window->Render();
	}

	// Clean up and shut down the engine
	Cleanup();

	return 0;
}
