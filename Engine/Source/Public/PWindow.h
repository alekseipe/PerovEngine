#pragma once

// System Headers
#include "EngineTypes.h"
#include "Math/PSTransform.h"

class PGraphicsEngine;
class PInput;

// Structure to hold window parameters
struct PSWindowParams
{
	// Default constructor with default window settings
	PSWindowParams()
		: title("Perov Engine Window"), x(0), y(0), w(1280), h(720), vsync(false), fullscreen(false) {}

	// Constructor with custom settings
	PSWindowParams(PString title, int x, int y, unsigned int w, unsigned int h)
		: title(title), x(x), y(y), w(w), h(h), vsync(false), fullscreen(false) {}

	PString title; // Title of the window
	int x, y; // Position of the window
	unsigned int w, h; // Width and height of the window
	bool vsync; // VSync enable flag
	bool fullscreen; // Fullscreen enable flag
};

struct SDL_Window;

class PWindow
{
public:
	PWindow();
	~PWindow();

	// Create the window with specified parameters
	bool CreateWindow(const PSWindowParams& params);

	// Close the window
	void CloseWindow() { m_ShouldClose = true; }

	// Check if the window is pending close
	bool IsPendingClose() const { return m_ShouldClose; }

	// Register input events
	void RegisterInput(const TShared<PInput>& input);

	// Render the graphics engine
	void Render();

private:
	SDL_Window* m_SDLWindow; // SDL window reference
	PSWindowParams m_Params; // Window parameters
	bool m_ShouldClose; // Flag to determine if the window should close
	TUnique<PGraphicsEngine> m_GraphicsEngine; // Graphics engine instance
	glm::vec3 m_CameraDirection; // Camera movement direction
	glm::vec3 m_CameraRotation; // Camera rotation amount
	bool m_CanZoom; // Zoom capability flag
	bool m_InputMode; // User input mode flag
};