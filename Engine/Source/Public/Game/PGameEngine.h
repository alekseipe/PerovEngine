#pragma once
#include "EngineTypes.h"
#include "PWindow.h"
#include "Listeners/PInput.h"

// External Headers
#include <SDL/SDL.h>

class PObject;

class PGameEngine
{
public:
	// Retrieve the existing game engine instance or create one if it doesn't exist
	static PGameEngine* GetGameEngine();

	// Safely destroy the game engine instance
	static void DestroyEngine();

	// Execute the game engine's main loop
	bool Run();

	// Return the time difference between frames in seconds
	double DeltaTime() const { return m_DeltaTime; }

	// Return the time difference between frames as a float
	float DeltaTimeF() const { return static_cast<float>(m_DeltaTime); }

	// Create a game object of type PObject or derived from it
	template<typename T, std::enable_if_t<std::is_base_of_v<PObject, T>, T>* = nullptr>
	TWeak<T> CreateObject()
	{
		// Instantiate an object using the template class
		TShared<T> newObject = TMakeShared<T>();

		// Add the newly created object to the instantiation stack
		m_ObjectsToBeInstantiated.push_back(newObject);

		return newObject;
	}

	// Mark an object for destruction
	// All game objects' destroy functions will automatically invoke this
	void DestroyObject(const TShared<PObject>& object);

private:
	// Private constructor and destructor to enforce a single instance of the game engine
	PGameEngine();
	~PGameEngine();

	// Initialize all necessary Headers and components for the game
	bool Initialise();

	// Function to run once the engine is initialized
	void Start();

	// Main game loop execution
	void GameLoop();

	// Clean up resources when the game engine shuts down
	void Cleanup();

	// LOOP FUNCTIONS
	// Run game logic for the current frame
	void Tick();

	// Handle input processing for the current frame
	void ProcessInput();

	// Render the graphics for the current frame
	void Render();

	// Operations to perform at the beginning of each loop
	void PreLoop();

	// Operations to perform at the end of each loop
	void PostLoop();

	// Store the game window instance
	TShared<PWindow> m_Window;

	// Store the input manager for the game
	TShared<PInput> m_Input;

	// Time of the last tick in seconds
	double m_LastTickTime;

	// Time difference between the current and previous frame in seconds
	double m_DeltaTime;

	// Stack of all PObjects currently in the game
	TArray<TShared<PObject>> m_ObjectStack;

	// Stack of PObjects scheduled to be instantiated in the next frame
	TArray<TShared<PObject>> m_ObjectsToBeInstantiated;

	// Stack of objects that have been marked for destruction
	TArray<TShared<PObject>> m_ObjectsPendingDestroy;
};
