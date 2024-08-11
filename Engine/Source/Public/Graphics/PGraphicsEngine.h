#pragma once
#include "EngineTypes.h"
#include "Graphics/PSMaterial.h"

typedef void* SDL_GLContext;
struct SDL_Window;
class PShaderProgram;
struct PSCamera;
struct PSLight;
struct PSPointLight;
struct PSDirLight;
class PModel;
struct PSSpotLight;

class PGraphicsEngine
{
public:
	PGraphicsEngine() = default;
	~PGraphicsEngine() = default;

	// Initialize the graphics engine
	bool InitEngine(SDL_Window* sdlWindow, const bool& vsync);

	// Render the graphics using the engine
	void Render(SDL_Window* sdlWindow);

	// Get a weak reference to the camera
	TWeak<PSCamera> GetCamera() { return m_Camera; }

	// Create and return a weak pointer to a point light
	TWeak<PSPointLight> CreatePointLight();

	// Create and return a weak pointer to a directional light
	TWeak<PSDirLight> CreateDirLight();

	// Create and return a weak pointer to a spotlight
	TWeak<PSSpotLight> CreateSpotLight();

	// Import a 3D model and return a weak pointer to it
	TWeak<PModel> ImportModel(const PString& path);

	// Create a material for the engine and return a shared pointer to it
	TShared<PSMaterial> CreateMaterial();

	// Calculate translation offset based on model rotation
	void TranslateOffModelRotation(float pointX, float pointZ, float degrees, float& objectX, float& objectZ);

	// Toggle the flashlight on or off
	void ToggleFlashlight();

	// Increase the red component of the flashlight's color
	void IncreaseFlashlightRed();

	// Increase the green component of the flashlight's color
	void IncreaseFlashlightGreen();

	// Increase the blue component of the flashlight's color
	void IncreaseFlashlightBlue();

	// Decrease the red component of the flashlight's color
	void DecreaseFlashlightRed();

	// Decrease the green component of the flashlight's color
	void DecreaseFlashlightGreen();

	// Decrease the blue component of the flashlight's color
	void DecreaseFlashlightBlue();

	// Increase the inner radius of the flashlight's beam
	void IncreaseFlashlightInnerRadius();

	// Increase the outer radius of the flashlight's beam
	void IncreaseFlashlightOuterRadius();

	// Decrease the inner radius of the flashlight's beam
	void DecreaseFlashlightInnerRadius();

	// Decrease the outer radius of the flashlight's beam
	void DecreaseFlashlightOuterRadius();

	// Increase the intensity of the flashlight
	void IncreaseFlashlightIntensity();

	// Decrease the intensity of the flashlight
	void DecreaseFlashlightIntensity();

private:
	// Store the OpenGL context for rendering
	SDL_GLContext m_SDLGLContext;

	// Store the shader program used by the engine
	TShared<PShaderProgram> m_Shader;

	// Store the camera instance
	TShared<PSCamera> m_Camera;

	// Store all the lights in the scene
	TArray<TShared<PSLight>> m_Lights;

	// Store all the models in the engine
	TArray<TShared<PModel>> m_Models;
};
