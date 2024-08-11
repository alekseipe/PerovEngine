#include "Game/GameObjects/PObject.h"

PObject::PObject()
{
	m_PendingDestroy = false;
	m_LifeTime = 0.0f;
	m_LifeTimeTimer = 0.0f;

	PDebug::Log("PObject created");
}

PObject::~PObject()
{
	PDebug::Log("PObject destroyed");
}

void PObject::Start()
{
	OnStart(); // Custom start logic for derived classes
}

void PObject::Tick(float deltaTime)
{
	OnTick(deltaTime); // Custom tick logic for derived classes

	// Handle object lifetime countdown
	if (m_LifeTime > 0.0f)
	{
		m_LifeTimeTimer -= deltaTime;

		// Destroy the object when its lifetime expires
		if (m_LifeTimeTimer <= 0.0f)
			Destroy();
	}
}

void PObject::PostTick(float deltaTime)
{
	OnPostTick(deltaTime); // Custom post-tick logic for derived classes
}

void PObject::Destroy()
{
	m_PendingDestroy = true;

	// Mark the object for destruction in the game engine
	PGameEngine::GetGameEngine()->DestroyObject(shared_from_this());
}
