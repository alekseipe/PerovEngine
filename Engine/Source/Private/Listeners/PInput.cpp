// Internal headers
#include "Listeners/PInput.h"
#include "PWindow.h"

PInput::PInput()
{
	OnKeyPress = TMakeShared<PEvents<SDL_Scancode>>();
	OnKeyRelease = TMakeShared<PEvents<SDL_Scancode>>();
	OnMouseMove = TMakeShared<PEvents<float, float, float, float>>();
	OnMouseScroll = TMakeShared<PEvents<float>>();
	OnMousePress = TMakeShared<PEvents<PUi8>>();
	OnMouseRelease = TMakeShared<PEvents<PUi8>>();
	m_LastMotion = SDL_MouseMotionEvent();
}

void PInput::InitInput(const TShared<PWindow>& window)
{
	// Assign a weak pointer from the shared pointer
	m_Window = window;
}

void PInput::UpdateInputs()
{
	// Event loop to handle SDL events
	SDL_Event e;
	bool mouseMoved = false;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			if (const auto& windowRef = m_Window.lock())
				windowRef->CloseWindow();
		}

		// Handle different types of input events
		switch (e.type)
		{
		case SDL_KEYDOWN:
			// Handle key press event if the key is not already being held down
			if (e.key.repeat == 0)
				OnKeyPress->Run(e.key.keysym.scancode);
			break;
		case SDL_KEYUP:
			// Handle key release event if the key is not already being held down
			if (e.key.repeat == 0)
				OnKeyRelease->Run(e.key.keysym.scancode);
			break;
		case SDL_MOUSEMOTION:
			// Handle mouse movement event
			OnMouseMove->Run(
				static_cast<float>(e.motion.x),
				static_cast<float>(e.motion.y),
				static_cast<float>(e.motion.xrel),
				static_cast<float>(e.motion.yrel)
			);
			// Store the last motion event for detecting when the mouse stops moving
			m_LastMotion = e.motion;
			mouseMoved = true;
			break;
		case SDL_MOUSEWHEEL:
			// Handle mouse wheel scroll event
			OnMouseScroll->Run(e.wheel.preciseY);
			break;
		case SDL_MOUSEBUTTONDOWN:
			// Handle mouse button press event
			OnMousePress->Run(e.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			// Handle mouse button release event
			OnMouseRelease->Run(e.button.button);
			break;
		default:
			break;
		}
	}

	// If the mouse hasn't moved and the last motion event was movement,
	// run the mouse move function with zero relative movement
	if (!mouseMoved && (m_LastMotion.xrel != 0 || m_LastMotion.yrel != 0))
	{
		OnMouseMove->Run(
			static_cast<float>(m_LastMotion.x),
			static_cast<float>(m_LastMotion.y),
			0.0f, 0.0f
		);

		// Reset relative motion values
		m_LastMotion.xrel = 0;
		m_LastMotion.yrel = 0;
	}
}

bool PInput::IsCursorHidden() const
{
	// Return true if the cursor is hidden, false otherwise
	return SDL_GetRelativeMouseMode() == SDL_TRUE;
}
void PInput::ShowCursor(const bool& enable)
{
	// Show or hide the cursor based on the enable flag
	SDL_SetRelativeMouseMode(enable ? SDL_FALSE : SDL_TRUE);
}
