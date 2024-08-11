#include "Listeners/PInput.h"
#include "PWindow.h"

PInput::PInput() {
	OnKeyPress = TMakeShared<PEvents<SDL_Scancode>>();
	OnKeyRelease = TMakeShared<PEvents<SDL_Scancode>>();
	OnMouseMove = TMakeShared<PEvents<float, float, float, float>>();
	OnMouseScroll = TMakeShared<PEvents<float>>();
	OnMousePress = TMakeShared<PEvents<PUi8>>();
	OnMouseRelease = TMakeShared<PEvents<PUi8>>();
	m_LastMotion = SDL_MouseMotionEvent();
}

void PInput::InitInput(const TShared<PWindow>& window) {
	// Assign a weak pointer from the shared pointer
	m_Window = window;
}

void PInput::UpdateInputs() {
	SDL_Event e;
	bool mouseMoved = false;

	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			if (const auto& windowRef = m_Window.lock())
				windowRef->CloseWindow();
		}

		// Handle different events based on input type
		switch (e.type) {
		case SDL_KEYDOWN:
			// If the key is not already being held down
			if (e.key.repeat == 0)
				OnKeyPress->Run(e.key.keysym.scancode);
			break;
		case SDL_KEYUP:
			if (e.key.repeat == 0)
				OnKeyRelease->Run(e.key.keysym.scancode);
			break;
		case SDL_MOUSEMOTION:
			// Run the mouse move event when the mouse moves
			OnMouseMove->Run(
				static_cast<float>(e.motion.x),
				static_cast<float>(e.motion.y),
				static_cast<float>(e.motion.xrel),
				static_cast<float>(e.motion.yrel)
			);
			// Store the last motion for stopping
			m_LastMotion = e.motion;
			mouseMoved = true;
			break;
		case SDL_MOUSEWHEEL:
			// Run the scroll event when the mouse wheel scrolls
			OnMouseScroll->Run(e.wheel.preciseY);
			break;
		case SDL_MOUSEBUTTONDOWN:
			// Run the pressed event when a mouse button is pressed
			OnMousePress->Run(e.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			// Run the release event when a mouse button is released
			OnMouseRelease->Run(e.button.button);
			break;
		default:
			break;
		}
	}

	// If the mouse hasn't moved and the last motion was movement
	// Run the mouse move function and zero out relative movements
	if (!mouseMoved && (m_LastMotion.xrel != 0 || m_LastMotion.yrel != 0)) {
		OnMouseMove->Run(
			static_cast<float>(m_LastMotion.x),
			static_cast<float>(m_LastMotion.y),
			0.0f, 0.0f
		);
		m_LastMotion.xrel = 0;
		m_LastMotion.yrel = 0;
	}
}

void PInput::ShowCursor(const bool& enable) {
	// Hide the mouse and set the cursor to the center of the screen if true
	SDL_SetRelativeMouseMode(enable ? SDL_FALSE : SDL_TRUE);
}

bool PInput::IsCursorHidden() const {
	return SDL_GetRelativeMouseMode() == SDL_TRUE ? true : false;
}
