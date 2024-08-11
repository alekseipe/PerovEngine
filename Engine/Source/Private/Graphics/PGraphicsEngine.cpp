#include "Graphics/PGraphicsEngine.h"
#include "Graphics/PModel.h"
#include "Graphics/PShaderProgram.h"
#include "Math/PSTransform.h"
#include "Graphics/PTexture.h"
#include "Graphics/PSCamera.h"
#include "Graphics/PSLight.h"

// External headers
#include <GLEW/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

// Model objects
TWeak<PModel> m_Crate;
TWeak<PModel> m_House;
TWeak<PModel> m_Heart;
TWeak<PSPointLight> m_PointLight;
TWeak<PSPointLight> m_PointLight2;
TWeak<PSSpotLight> m_SpotLight;
TWeak<PSSpotLight> m_SpotLight2;
TWeak<PSSpotLight> m_Flashlight;

const std::vector<PSVertexData> vertexData = {
	{ {-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} },
	{ { 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} },
	{ {-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} },
	{ { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} }
};

const std::vector<uint32_t> indexData = {
	 0, 1, 2, // Triangle 1
	 1, 2, 3  // Triangle 2
};

const double pi = 3.14159265358979323846;

bool PGraphicsEngine::InitEngine(SDL_Window* sdlWindow, const bool& vsync)
{
	if (sdlWindow == nullptr)
	{
		PDebug::Log("SDL window is null", LT_ERROR);
		PDebug::Log("Failed to initialize graphics engine", LT_ERROR);
		return false;
	}

	// Create OpenGL context
	m_SDLGLContext = SDL_GL_CreateContext(sdlWindow);

	// Check if context creation failed
	if (m_SDLGLContext == nullptr)
	{
		PDebug::Log("SDL failed to create OpenGL context: " + std::string(SDL_GetError()), LT_ERROR);
		PDebug::Log("Failed to initialize graphics engine", LT_ERROR);
		return false;
	}

	if (SDL_GL_MakeCurrent(sdlWindow, m_SDLGLContext) != 0)
	{
		PDebug::Log("SDL failed to make OpenGL context current: " + std::string(SDL_GetError()), LT_ERROR);
		PDebug::Log("Failed to initialize graphics engine", LT_ERROR);
		return false;
	}

	if (vsync)
	{
		// Attempt to enable adaptive vsync
		if (SDL_GL_SetSwapInterval(-1) != 0)
		{
			// Fallback to standard vsync
			if (SDL_GL_SetSwapInterval(1) != 0)
			{
				PDebug::Log("Failed to initialize vsync: " + std::string(SDL_GetError()), LT_WARN);
				return false;
			}
		}
	}

	// Initialize GLEW
	GLenum glewResult = glewInit();

	// Check if GLEW initialization failed
	if (glewResult != GLEW_OK)
	{
		std::string errorMsg = reinterpret_cast<const char*>(glewGetErrorString(glewResult));
		PDebug::Log("Failed to initialize GLEW: " + errorMsg);
		return false;
	}
	glEnable(GL_DEPTH_TEST);

	// Create and initialize shader
	m_Shader = TMakeShared<PShaderProgram>();
	if (!m_Shader->InitShader("Shaders/SimpleShader/SimpleShader.vertex", "Shaders/SimpleShader/SimpleShader.frag"))
	{
		PDebug::Log("Shader initialization failed");
		return false;
	}

	// Create camera
	m_Camera = TMakeShared<PSCamera>();
	m_Camera->transform.position.y = 25.0f;
	m_Camera->transform.position.z = -75.0f;

	// Load models and apply materials

	// Crate model
	m_Crate = ImportModel("Models/SimpleCrate/Crate.fbx");
	m_Crate.lock()->GetTransform().position.z = 200.0f;
	m_Crate.lock()->GetTransform().rotation.y = 180.0f;

	TShared<PTexture> crateTex = TMakeShared<PTexture>();
	TShared<PTexture> crateSpecTex = TMakeShared<PTexture>();
	TShared<PTexture> crateNormTex = TMakeShared<PTexture>();
	crateTex->LoadTexture("Crate base color", "Models/SimpleCrate/textures/crate_txt.png");
	crateSpecTex->LoadTexture("Crate specular", "Models/SimpleCrate/textures/crate_txt.png");
	crateNormTex->LoadTexture("Crate normal", "Models/SimpleCrate/textures/crate_txt.png");

	TShared<PSMaterial> crateMat = TMakeShared<PSMaterial>();
	crateMat->m_BaseColourMap = crateTex;
	crateMat->m_SpecularMap = crateSpecTex;
	crateMat->m_NormalMap = crateNormTex;
	m_Crate.lock()->SetMaterialBySlot(0, crateMat);

	// House model
	m_House = ImportModel("Models/House/house.fbx");
	TShared<PTexture> houseTex = TMakeShared<PTexture>();
	TShared<PTexture> houseTex2 = TMakeShared<PTexture>();
	TShared<PTexture> houseTex3 = TMakeShared<PTexture>();
	TShared<PTexture> houseNormTex = TMakeShared<PTexture>();
	TShared<PTexture> houseNormTex2 = TMakeShared<PTexture>();
	TShared<PTexture> houseNormTex3 = TMakeShared<PTexture>();
	TShared<PSMaterial> houseMat = CreateMaterial();
	TShared<PSMaterial> houseMat2 = CreateMaterial();
	TShared<PSMaterial> houseMat3 = CreateMaterial();
	houseTex->LoadTexture("Window color", "Models/House/textures/BaseColor.png");
	houseTex2->LoadTexture("Door color", "Models/House/textures/Scene_-_Root_baseColor.png");
	houseTex3->LoadTexture("Main color", "Models/House/textures/Rusty_BaseColor.png");

	houseNormTex->LoadTexture("Window normal map", "Models/House/textures/Normal.png");
	houseNormTex2->LoadTexture("Door normal map", "Models/House/textures/Scene_-_Root_normal.png");
	houseNormTex3->LoadTexture("Main normal map", "Models/House/textures/walls_base_normal.png");

	houseMat->m_BaseColourMap = houseTex;
	houseMat->m_NormalMap = houseNormTex;
	houseMat->shininess = 100.0f;
	houseMat->specularStrength = 0.1f;
	houseMat2->m_BaseColourMap = houseTex2;
	houseMat->m_NormalMap = houseNormTex2;
	houseMat2->shininess = 100.0f;
	houseMat2->specularStrength = 0.1f;
	houseMat3->m_BaseColourMap = houseTex3;
	houseMat->m_NormalMap = houseNormTex3;
	houseMat3->shininess = 100.0f;
	houseMat3->specularStrength = 0.1f;
	m_House.lock()->SetMaterialBySlot(0, houseMat);
	m_House.lock()->SetMaterialBySlot(1, houseMat2);
	m_House.lock()->SetMaterialBySlot(2, houseMat3);

	// Heart model
	m_Heart = ImportModel("Models/Heart/Heart.fbx");
	m_Heart.lock()->GetTransform().position.z = 10.0f;
	m_Heart.lock()->GetTransform().position.y = 20.0f;
	m_Heart.lock()->GetTransform().rotation.z = 10.0f;

	TShared<PTexture> heartTex = TMakeShared<PTexture>();
	TShared<PTexture> heartSpecTex = TMakeShared<PTexture>();
	TShared<PTexture> heartNormTex = TMakeShared<PTexture>();
	TShared<PSMaterial> heartMat = CreateMaterial();
	heartTex->LoadTexture("Heart base color", "Models/Heart/textures/red.png");
	heartSpecTex->LoadTexture("Heart specular", "Models/Heart/textures/red.png");
	heartNormTex->LoadTexture("Heart normal", "Models/Heart/textures/red.png");
	heartMat->m_BaseColourMap = heartTex;
	heartMat->m_SpecularMap = heartSpecTex;
	heartMat->m_NormalMap = heartNormTex;
	m_Heart.lock()->SetMaterialBySlot(0, heartMat);

	// Create directional light
	const auto& dirLight = CreateDirLight();
	if (const auto& lightRef = dirLight.lock())
	{
		lightRef->colour = glm::vec3(1.0f, 0.0f, 0.0f);
		lightRef->intensity = 0.1f;
		lightRef->direction = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	// Create point lights
	m_PointLight = CreatePointLight();
	if (const auto& lightRef = m_PointLight.lock())
	{
		m_PointLight.lock()->colour = glm::vec3(1.0f, 0.5f, 0.0f);
		m_PointLight.lock()->linear = 0.007f;
		m_PointLight.lock()->quadratic = 0.0002f;
	}

	m_PointLight2 = CreatePointLight();
	if (const auto& lightRef = m_PointLight2.lock())
	{
		m_PointLight2.lock()->position.x = 40.0f;
		m_PointLight2.lock()->position.y = 70.0f;
		m_PointLight2.lock()->position.z = 115.0f;
		m_PointLight2.lock()->colour = glm::vec3(1.0f, 0.0f, 1.0f);
		m_PointLight2.lock()->linear = 0.007f;
		m_PointLight2.lock()->quadratic = 0.0002f;
	}

	// Create spotlights
	m_SpotLight = CreateSpotLight();
	if (const auto& lightRef = m_SpotLight.lock())
	{
		m_SpotLight.lock()->SetInnerCutOff(3.5f);
		m_SpotLight.lock()->SetOuterCutOff(7.5f);
		m_SpotLight.lock()->colour = glm::vec3(0.0f, 1.0f, 0.0f);
		m_SpotLight.lock()->linear = 0.0014f;
		m_SpotLight.lock()->quadratic = 0.000007f;
	}

	m_SpotLight2 = CreateSpotLight();
	if (const auto& lightRef = m_SpotLight2.lock())
	{
		m_SpotLight2.lock()->SetInnerCutOff(3.5f);
		m_SpotLight2.lock()->SetOuterCutOff(7.5f);
		m_SpotLight2.lock()->colour = glm::vec3(1.0f, 0.0f, 0.0f);
		m_SpotLight2.lock()->linear = 0.0014f;
		m_SpotLight2.lock()->quadratic = 0.000007f;
	}

	// Flashlight
	m_Flashlight = CreateSpotLight();
	if (const auto& lightRef = m_Flashlight.lock())
	{
		m_Flashlight.lock()->linear = 0.0014f;
		m_Flashlight.lock()->quadratic = 0.000007f;
	}

	// Log success
	PDebug::Log("Graphics engine successfully initialized", LT_SUCCESS);

	return true;
}

void PGraphicsEngine::Render(SDL_Window* sdlWindow)
{
	// Set background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Clear the back buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Rotate point light 2 around the crate
	TranslateOffModelRotation(m_Crate.lock()->GetTransform().position.x, m_Crate.lock()->GetTransform().position.z,
		-1.0f, m_PointLight2.lock()->position.x, m_PointLight2.lock()->position.z);


	// Rotate heart model
	m_Heart.lock()->GetTransform().rotation.y += 1.00f;

	// Adjust spotlight size
	static float radiusChangeRate = 0.08f;

	// Stop increasing if size is reached
	if (m_SpotLight.lock()->innerDegrees >= 2.0f)
	{
		radiusChangeRate = -0.05f;
	}
	else if (m_SpotLight.lock()->innerDegrees <= 0.5f)
	{
		radiusChangeRate = 0.05f;
	}

	m_SpotLight.lock()->SetInnerCutOff(m_SpotLight.lock()->innerDegrees += radiusChangeRate);
	m_SpotLight.lock()->SetOuterCutOff(m_SpotLight.lock()->outerDegrees += radiusChangeRate);
	m_SpotLight2.lock()->SetInnerCutOff(m_SpotLight2.lock()->innerDegrees += radiusChangeRate);
	m_SpotLight2.lock()->SetOuterCutOff(m_SpotLight2.lock()->outerDegrees += radiusChangeRate);

	// Position flashlight with camera
	m_Flashlight.lock()->position = m_Camera->transform.position;
	m_Flashlight.lock()->direction = m_Camera->transform.Forward();

	// Activate shader and render models
	m_Shader->Activate();
	m_Shader->SetWorldTransform(m_Camera);
	for (const auto& modelRef : m_Models)
	{
		modelRef->Render(m_Shader, m_Lights);
	}

	// Swap buffers
	SDL_GL_SwapWindow(sdlWindow);
}

TWeak<PSPointLight> PGraphicsEngine::CreatePointLight()
{
	const auto& newLight = TMakeShared<PSPointLight>();
	m_Lights.push_back(newLight);
	return newLight;
}

TWeak<PSDirLight> PGraphicsEngine::CreateDirLight()
{
	const auto& newLight = TMakeShared<PSDirLight>();
	m_Lights.push_back(newLight);
	return newLight;
}

TWeak<PSSpotLight> PGraphicsEngine::CreateSpotLight()
{
	const auto& newLight = TMakeShared<PSSpotLight>();
	m_Lights.push_back(newLight);
	return newLight;
}

TWeak<PModel> PGraphicsEngine::ImportModel(const PString& path)
{
	const auto& newModel = TMakeShared<PModel>();
	newModel->ImportModel(path);
	m_Models.push_back(newModel);
	return newModel;
}

TShared<PSMaterial> PGraphicsEngine::CreateMaterial()
{
	return TMakeShared<PSMaterial>();
}

void PGraphicsEngine::TranslateOffModelRotation(float pointX, float pointZ, float degrees, float& objectX, float& objectZ)
{
	float newX = 0.0f;
	float newZ = 0.0f;

	float finalX = 0.0f;
	float finalZ = 0.0f;

	float radians = degrees * pi / 180;

	// Translate
	newX = objectX - pointX;
	newZ = objectZ - pointZ;

	// Rotate
	finalX = (newX * cos(radians)) - (newZ * sin(radians));
	finalZ = (newX * sin(radians)) + (newZ * cos(radians));

	// Translate to final point
	objectX = finalX + pointX;
	objectZ = finalZ + pointZ;
}

void PGraphicsEngine::ToggleFlashlight()
{
	if (m_Flashlight.lock()->intensity > 0)
		m_Flashlight.lock()->intensity = 0.0f;
	else
		m_Flashlight.lock()->intensity = 1.0f;
}

void PGraphicsEngine::IncreaseFlashlightRed()
{
	if (m_Flashlight.lock()->colour.r >= 1.0f)
	{
		return;
	}

	m_Flashlight.lock()->colour = glm::vec3(
		m_Flashlight.lock()->colour.r + 0.1f,
		m_Flashlight.lock()->colour.g,
		m_Flashlight.lock()->colour.b);
}

void PGraphicsEngine::IncreaseFlashlightGreen()
{
	if (m_Flashlight.lock()->colour.r >= 1.0f)
	{
		return;
	}

	m_Flashlight.lock()->colour = glm::vec3(
		m_Flashlight.lock()->colour.g,
		m_Flashlight.lock()->colour.g + 0.1f,
		m_Flashlight.lock()->colour.b);
}

void PGraphicsEngine::IncreaseFlashlightBlue()
{
	if (m_Flashlight.lock()->colour.b >= 1.0f)
	{
		return;
	}

	m_Flashlight.lock()->colour = glm::vec3(
		m_Flashlight.lock()->colour.r,
		m_Flashlight.lock()->colour.g,
		m_Flashlight.lock()->colour.b + 0.1f);
}

void PGraphicsEngine::DecreaseFlashlightRed()
{
	if (m_Flashlight.lock()->colour.r <= 0.0f)
	{
		return;
	}

	m_Flashlight.lock()->colour = glm::vec3(
		m_Flashlight.lock()->colour.r - 0.1f,
		m_Flashlight.lock()->colour.g,
		m_Flashlight.lock()->colour.b);
}

void PGraphicsEngine::DecreaseFlashlightGreen()
{
	if (m_Flashlight.lock()->colour.g <= 0.0f)
	{
		return;
	}

	m_Flashlight.lock()->colour = glm::vec3(
		m_Flashlight.lock()->colour.r,
		m_Flashlight.lock()->colour.g - 0.1f,
		m_Flashlight.lock()->colour.b);
}

void PGraphicsEngine::DecreaseFlashlightBlue()
{
	if (m_Flashlight.lock()->colour.b <= 0.0f)
	{
		return;
	}

	m_Flashlight.lock()->colour = glm::vec3(
		m_Flashlight.lock()->colour.r,
		m_Flashlight.lock()->colour.g,
		m_Flashlight.lock()->colour.b - 0.1f);
}

void PGraphicsEngine::IncreaseFlashlightInnerRadius()
{
	m_Flashlight.lock()->SetInnerCutOff(m_Flashlight.lock()->innerDegrees + 1);
}

void PGraphicsEngine::IncreaseFlashlightOuterRadius()
{
	m_Flashlight.lock()->SetOuterCutOff(m_Flashlight.lock()->outerDegrees + 1);
}

void PGraphicsEngine::DecreaseFlashlightInnerRadius()
{
	m_Flashlight.lock()->SetInnerCutOff(m_Flashlight.lock()->innerDegrees - 1);
}

void PGraphicsEngine::DecreaseFlashlightOuterRadius()
{
	m_Flashlight.lock()->SetOuterCutOff(m_Flashlight.lock()->outerDegrees - 1);
}

void PGraphicsEngine::IncreaseFlashlightIntensity()
{
	m_Flashlight.lock()->intensity += 0.1f;
}

void PGraphicsEngine::DecreaseFlashlightIntensity()
{
	m_Flashlight.lock()->intensity -= 0.1f;
}
