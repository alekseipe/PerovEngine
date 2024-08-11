#pragma once
#include "EngineTypes.h"
#include "Listeners/PEvents.h"

// External libraries
#include <SDL/SDL_keycode.h>
#include <SDL/SDL_events.h>

class PWindow;

class PInput
{
public:
	PInput();

	// Initialize input handling for the given window
	void InitInput(const TShared<PWindow>& window);

	// Update and process input events
	void UpdateInputs();

	// Event triggered when a key is pressed
	TShared<PEvents<SDL_Scancode>> OnKeyPress;

	// Event triggered when a key is released
	TShared<PEvents<SDL_Scancode>> OnKeyRelease;

	// Event triggered on mouse movement
	// @params 1: X coordinate of the mouse
	// @params 2: Y coordinate of the mouse
	// @params 3: Relative movement on the X axis
	// @params 4: Relative movement on the Y axis
	TShared<PEvents<float, float, float, float>> OnMouseMove;

	// Event triggered when the mouse scrolls, passing the delta value
	TShared<PEvents<float>> OnMouseScroll;

	// Event triggered when a mouse button is pressed
	TShared<PEvents<PUi8>> OnMousePress;

	// Event triggered when a mouse button is released
	TShared<PEvents<PUi8>> OnMouseRelease;

	// Show or hide the cursor
	// Hiding the cursor also keeps it centered on the screen while moving
	void ShowCursor(const bool& enable);

	// Check if the cursor is currently hidden
	bool IsCursorHidden() const;

private:
	// Weak pointer to the window, preventing it from influencing the window's lifetime
	TWeak<PWindow> m_Window;

	// Store the last mouse motion event
	SDL_MouseMotionEvent m_LastMotion;
};
