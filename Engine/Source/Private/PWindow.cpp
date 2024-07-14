// Internal headers
#include "PWindow.h"
#include "Graphics/PGraphicsEngine.h"
#include "Debug/PDebug.h"
#include "Listeners/PInput.h"
#include "Graphics/PSCamera.h"

// External libraries
#include <SDL/SDL.h>

PWindow::PWindow()
{
	m_SDLWindow = nullptr;
	m_ShouldClose = false;
	m_CameraDirection = glm::vec3(0.0f);
	m_CameraRotation = glm::vec3(0.0f);
	m_CanZoom = false;
	m_InputMode = false;

	std::cout << "Window created" << std::endl;
}

PWindow::~PWindow()
{
	// Destroy the SDL window if it exists
	if (m_SDLWindow)
		SDL_DestroyWindow(m_SDLWindow);

	std::cout << "Window destroyed" << std::endl;
}

bool PWindow::CreateWindow(const PSWindowParams& params)
{
	// Enable OpenGL in the SDL window
	unsigned int windowFlags = SDL_WINDOW_OPENGL;

	// Store window parameters
	m_Params = params;

	// Add high DPI flag if vsync is enabled
	if (m_Params.vsync)
		windowFlags += SDL_WINDOW_ALLOW_HIGHDPI;

	// Set window mode (fullscreen or windowed)
	if (m_Params.fullscreen)
		windowFlags += SDL_WINDOW_FULLSCREEN_DESKTOP; // Fullscreen borderless
	else
		windowFlags += SDL_WINDOW_SHOWN; // Windowed mode

	// Create the SDL window
	m_SDLWindow = SDL_CreateWindow(
		m_Params.title.c_str(),
		m_Params.x,
		m_Params.y,
		m_Params.w,
		m_Params.h,
		windowFlags
	);

	// Check if the SDL window was created successfully
	if (!m_SDLWindow)
	{
		std::cout << "SDL failed to create window: " << SDL_GetError() << std::endl;
		CloseWindow();
		return false;
	}

	// Initialize the graphics engine
	m_GraphicsEngine = std::make_unique<PGraphicsEngine>();
	if (!m_GraphicsEngine->InitEngine(m_SDLWindow, m_Params.vsync))
	{
		PDebug::Log("Failed to initialize graphics engine", LT_ERROR);
		m_GraphicsEngine = nullptr;
		return false;
	}

	return true;
}

void PWindow::RegisterInput(const TShared<PInput>& m_Input)
{
	// Hide the cursor and set relative mouse mode
	m_Input->ShowCursor(false);

	// Bind key press events
	m_Input->OnKeyPress->Bind([this, m_Input](const SDL_Scancode& key)
		{
			// Handle quick exit for debug
			if (key == SDL_SCANCODE_ESCAPE)
				CloseWindow();

			// Toggle cursor visibility
			if (key == SDL_SCANCODE_PERIOD)
			{
				m_Input->ShowCursor(m_Input->IsCursorHidden());
				m_InputMode = !m_Input->IsCursorHidden();
			}

			// Handle camera movement input
			if (key == SDL_SCANCODE_W) // Forward
				m_CameraDirection.z += 1.0f;
			if (key == SDL_SCANCODE_S) // Backward
				m_CameraDirection.z += -1.0f;
			if (key == SDL_SCANCODE_A) // Left
				m_CameraDirection.x += -1.0f;
			if (key == SDL_SCANCODE_D) // Right
				m_CameraDirection.x += 1.0f;
			if (key == SDL_SCANCODE_Q) // Down
				m_CameraDirection.y += -1.0f;
			if (key == SDL_SCANCODE_E) // Up
				m_CameraDirection.y += 1.0f;
		});

	// Bind key release events
	m_Input->OnKeyRelease->Bind([this](const SDL_Scancode& key)
		{
			// Handle camera movement stop
			if (key == SDL_SCANCODE_W) // Forward
				m_CameraDirection.z += -1.0f;
			if (key == SDL_SCANCODE_S) // Backward
				m_CameraDirection.z += 1.0f;
			if (key == SDL_SCANCODE_A) // Left
				m_CameraDirection.x += 1.0f;
			if (key == SDL_SCANCODE_D) // Right
				m_CameraDirection.x += -1.0f;
			if (key == SDL_SCANCODE_Q) // Down
				m_CameraDirection.y += 1.0f;
			if (key == SDL_SCANCODE_E) // Up
				m_CameraDirection.y += -1.0f;
		});

	// Bind mouse movement events to rotate the camera
	m_Input->OnMouseMove->Bind([this](const float& x, const float& y,
		const float& xrel, const float& yrel)
		{
			m_CameraRotation.y = -xrel;
			m_CameraRotation.x = -yrel;
		});

	// Bind mouse scroll events for zooming
	m_Input->OnMouseScroll->Bind([this](const float& delta)
		{
			if (m_CanZoom)
			{
				if (const auto& camRef = m_GraphicsEngine->GetCamera().lock())
				{
					camRef->Zoom(delta);
				}
			}
		});

	// Bind mouse press events
	m_Input->OnMousePress->Bind([this](const PUi8& button)
		{
			if (button == SDL_BUTTON_RIGHT)
			{
				m_CanZoom = true;
			}
		});

	// Bind mouse release events
	m_Input->OnMouseRelease->Bind([this](const PUi8& button)
		{
			if (button == SDL_BUTTON_RIGHT)
			{
				m_CanZoom = false;
				if (const auto& camRef = m_GraphicsEngine->GetCamera().lock())
				{
					camRef->ResetZoom();
				}
			}
		});
}

void PWindow::Render()
{
	// Render using the graphics engine if available
	if (m_GraphicsEngine)
	{
		// Update the camera if available
		if (const auto& camRef = m_GraphicsEngine->GetCamera().lock())
		{
			if (!m_InputMode)
			{
				// Translate and rotate the camera based on input
				camRef->Translate(m_CameraDirection);
				camRef->Rotate(m_CameraRotation, glm::abs(m_CameraRotation));
			}
		}
		m_GraphicsEngine->Render(m_SDLWindow);
	}
}
