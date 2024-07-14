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

	// Initialize the input system with a window
	void InitInput(const TShared<PWindow>& window);

	// Update the inputs (called every frame)
	void UpdateInputs();

	// Event triggered on key press
	TShared<PEvents<SDL_Scancode>> OnKeyPress;

	// Event triggered on key release
	TShared<PEvents<SDL_Scancode>> OnKeyRelease;

	// Event triggered on mouse movement
	// @params 1: mouse pos x
	// @params 2: mouse pos y
	// @params 3: x relative movement
	// @params 4: y relative movement
	TShared<PEvents<float, float, float, float>> OnMouseMove;

	// Event triggered when the mouse scrolls, passing the delta
	TShared<PEvents<float>> OnMouseScroll;

	// Event triggered on mouse button press
	TShared<PEvents<PUi8>> OnMousePress;

	// Event triggered on mouse button release
	TShared<PEvents<PUi8>> OnMouseRelease;

	// Show or hide the cursor
	// If hidden, the cursor will be centered on the screen when moving
	void ShowCursor(const bool& enable);

	// Check if the cursor is hidden
	bool IsCursorHidden() const;

private:
	// Weak pointer to the window to avoid preventing its destruction
	TWeak<PWindow> m_Window;

	// Last mouse motion event
	SDL_MouseMotionEvent m_LastMotion;
};
