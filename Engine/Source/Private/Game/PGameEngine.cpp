#include "Game/PGameEngine.h"

// DEBUG
#include "Game/GameObjects/PObjectChild.h"

PGameEngine* PGameEngine::GetGameEngine()
{
	static PGameEngine* instance = new PGameEngine();
	return instance;
}

void PGameEngine::DestroyEngine()
{
	delete GetGameEngine();
}

bool PGameEngine::Run()
{
	if (!Initialise())
	{
		PDebug::Log("Game Engine failed to initialise", LT_ERROR);
		return false;
	}

	Start();
	GameLoop();

	return true;
}

void PGameEngine::DestroyObject(const TShared<PObject>& object)
{
	m_ObjectsPendingDestroy.push_back(object);
}

PGameEngine::PGameEngine()
{
	m_LastTickTime = 0.0;
	m_DeltaTime = 0.0;
	PDebug::Log("Game Engine created");
}

PGameEngine::~PGameEngine()
{
	Cleanup();
	PDebug::Log("Game Engine destroyed");
}

bool PGameEngine::Initialise()
{
	// Initialize SDL components needed for the engine
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
	{
		PDebug::Log("Failed to init SDL: " + PString(SDL_GetError()), LT_ERROR);
		return false;
	}

	// Set OpenGL version attributes for the SDL window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);

	m_Window = TMakeShared<PWindow>();

	// Create an SDL window
	if (!m_Window->CreateWindow({ "Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 720 }))
		return false;

	// Create the input class and assign the window
	m_Input = TMakeShared<PInput>();
	m_Input->InitInput(m_Window);

	return true;
}

void PGameEngine::Start()
{
	// Register the window inputs
	m_Window->RegisterInput(m_Input);

	// Create and set the lifetime of an example game object
	CreateObject<PObjectChild>().lock()->SetLifeTime(5.0f);
}

void PGameEngine::GameLoop()
{
	// Keep the game running while the window is open
	while (!m_Window->IsPendingClose())
	{
		// Calculate delta time for the current frame
		double curTickTime = static_cast<double>(SDL_GetTicks64());
		double deltaMilli = curTickTime - m_LastTickTime;
		m_DeltaTime = deltaMilli / 1000.0;
		m_LastTickTime = curTickTime;

		PreLoop();    // Handle logic that needs to run before each frame
		ProcessInput(); // Process input for the frame
		Tick();       // Run game logic
		Render();     // Render the current frame
		PostLoop();   // Handle logic that needs to run after each frame
	}
}

void PGameEngine::Cleanup()
{
	m_Input = nullptr;
	m_Window = nullptr;
	SDL_Quit();
}

void PGameEngine::Tick()
{
	// Run the Tick and PostTick methods for all game objects
	for (const auto& pObjectRef : m_ObjectStack)
	{
		pObjectRef->Tick(DeltaTimeF());
		pObjectRef->PostTick(DeltaTimeF());
	}
}

void PGameEngine::ProcessInput()
{
	if (m_Input)
		m_Input->UpdateInputs();
}

void PGameEngine::Render()
{
	if (m_Window)
		m_Window->Render();
}

void PGameEngine::PreLoop()
{
	// Start all objects that need to be instantiated and add them to the object stack
	for (auto& pObjectRef : m_ObjectsToBeInstantiated)
	{
		pObjectRef->Start();
		m_ObjectStack.push_back(std::move(pObjectRef));
	}

	m_ObjectsToBeInstantiated.clear();
}

void PGameEngine::PostLoop()
{
	// Destroy objects marked for deletion and remove them from the object stack
	for (const auto& pObjectRef : m_ObjectsPendingDestroy)
	{
		auto it = std::find(m_ObjectStack.begin(), m_ObjectStack.end(), pObjectRef);
		if (it != m_ObjectStack.end())
			m_ObjectStack.erase(it);
	}

	m_ObjectsPendingDestroy.clear();
}
