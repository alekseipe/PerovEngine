#include "PWindow.h"
#include "Graphics/PGraphicsEngine.h"
#include "Debug/PDebug.h"
#include "Listeners/PInput.h"
#include "Graphics/PSCamera.h"

// External Headers
#include <SDL/SDL.h>

PWindow::PWindow() {
	m_SDLWindow = nullptr;
	m_ShouldClose = false;
	m_CameraDirection = glm::vec3(0.0f);
	m_CameraRotation = glm::vec3(0.0f);
	m_CanZoom = false;
	m_InputMode = false;
	std::cout << "Window created" << std::endl;
}

PWindow::~PWindow() {
	if (m_SDLWindow)
		SDL_DestroyWindow(m_SDLWindow);
	std::cout << "Window destroyed" << std::endl;
}

bool PWindow::CreateWindow(const PSWindowParams& params) {
	unsigned int windowFlags = SDL_WINDOW_OPENGL;
	m_Params = params;
	if (m_Params.vsync)
		windowFlags += SDL_WINDOW_ALLOW_HIGHDPI;
	if (m_Params.fullscreen)
		windowFlags += SDL_WINDOW_FULLSCREEN_DESKTOP;
	else
		windowFlags += SDL_WINDOW_SHOWN;

	m_SDLWindow = SDL_CreateWindow(
		m_Params.title.c_str(),
		m_Params.x,
		m_Params.y,
		m_Params.w,
		m_Params.h,
		windowFlags
	);

	if (!m_SDLWindow) {
		std::cout << "SDL failed to create window: " << SDL_GetError() << std::endl;
		CloseWindow();
		return false;
	}

	m_GraphicsEngine = std::make_unique<PGraphicsEngine>();

	if (!m_GraphicsEngine->InitEngine(m_SDLWindow, m_Params.vsync)) {
		PDebug::Log("Window failed to initialize graphics engine", LT_ERROR);
		m_GraphicsEngine = nullptr;
		return false;
	}

	return true;
}

void PWindow::RegisterInput(const TShared<PInput>& m_Input) {
	m_Input->ShowCursor(false);
	m_Input->OnKeyPress->Bind([this, m_Input](const SDL_Scancode& key) {
		if (key == SDL_SCANCODE_ESCAPE)
			CloseWindow();
		if (key == SDL_SCANCODE_PERIOD) {
			m_Input->ShowCursor(m_Input->IsCursorHidden());
			m_InputMode = !m_Input->IsCursorHidden();
		}
		if (key == SDL_SCANCODE_W)
			m_CameraDirection.z += 1.0f;
		if (key == SDL_SCANCODE_S)
			m_CameraDirection.z -= 1.0f;
		if (key == SDL_SCANCODE_A)
			m_CameraDirection.x -= 1.0f;
		if (key == SDL_SCANCODE_D)
			m_CameraDirection.x += 1.0f;
		if (key == SDL_SCANCODE_Q)
			m_CameraDirection.y -= 1.0f;
		if (key == SDL_SCANCODE_E)
			m_CameraDirection.y += 1.0f;
		if (key == SDL_SCANCODE_F)
			m_GraphicsEngine->ToggleFlashlight();
		if (key == SDL_SCANCODE_1)
			m_GraphicsEngine->IncreaseFlashlightRed();
		if (key == SDL_SCANCODE_2)
			m_GraphicsEngine->IncreaseFlashlightGreen();
		if (key == SDL_SCANCODE_3)
			m_GraphicsEngine->IncreaseFlashlightBlue();
		if (key == SDL_SCANCODE_4)
			m_GraphicsEngine->DecreaseFlashlightRed();
		if (key == SDL_SCANCODE_5)
			m_GraphicsEngine->DecreaseFlashlightGreen();
		if (key == SDL_SCANCODE_6)
			m_GraphicsEngine->DecreaseFlashlightBlue();
		if (key == SDL_SCANCODE_I)
			m_GraphicsEngine->IncreaseFlashlightInnerRadius();
		if (key == SDL_SCANCODE_O)
			m_GraphicsEngine->IncreaseFlashlightOuterRadius();
		if (key == SDL_SCANCODE_J)
			m_GraphicsEngine->DecreaseFlashlightInnerRadius();
		if (key == SDL_SCANCODE_K)
			m_GraphicsEngine->DecreaseFlashlightOuterRadius();
		if (key == SDL_SCANCODE_P)
			m_GraphicsEngine->IncreaseFlashlightIntensity();
		if (key == SDL_SCANCODE_L)
			m_GraphicsEngine->DecreaseFlashlightIntensity();
		});

	m_Input->OnKeyRelease->Bind([this](const SDL_Scancode& key) {
		if (key == SDL_SCANCODE_W)
			m_CameraDirection.z -= 1.0f;
		if (key == SDL_SCANCODE_S)
			m_CameraDirection.z += 1.0f;
		if (key == SDL_SCANCODE_A)
			m_CameraDirection.x += 1.0f;
		if (key == SDL_SCANCODE_D)
			m_CameraDirection.x -= 1.0f;
		if (key == SDL_SCANCODE_Q)
			m_CameraDirection.y += 1.0f;
		if (key == SDL_SCANCODE_E)
			m_CameraDirection.y -= 1.0f;
		});

	m_Input->OnMouseMove->Bind([this](const float& x, const float& y, const float& xrel, const float& yrel) {
		m_CameraRotation.y = -xrel;
		m_CameraRotation.x = -yrel;
		});

	m_Input->OnMouseScroll->Bind([this](const float& delta) {
		if (m_CanZoom) {
			if (const auto& camRef = m_GraphicsEngine->GetCamera().lock()) {
				camRef->Zoom(delta);
			}
		}
		});

	m_Input->OnMousePress->Bind([this](const PUi8& button) {
		if (button == SDL_BUTTON_RIGHT) {
			m_CanZoom = true;
		}
		});

	m_Input->OnMouseRelease->Bind([this](const PUi8& button) {
		if (button == SDL_BUTTON_RIGHT) {
			m_CanZoom = false;
			if (const auto& camRef = m_GraphicsEngine->GetCamera().lock()) {
				camRef->ResetZoom();
			}
		}
		});
}

void PWindow::Render() {
	if (m_GraphicsEngine) {
		if (const auto& camRef = m_GraphicsEngine->GetCamera().lock()) {
			if (!m_InputMode) {
				camRef->Translate(m_CameraDirection);
				camRef->Rotate(m_CameraRotation, glm::abs(m_CameraRotation));
			}
		}
		m_GraphicsEngine->Render(m_SDLWindow);
	}
}
