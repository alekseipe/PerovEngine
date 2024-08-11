#pragma once
#include "EngineTypes.h"
#include "Game/PGameEngine.h"

// PObject can use shared_from_this() as a replacement for the 'this' keyword when working with shared pointers
class PObject : public std::enable_shared_from_this<PObject>
{
public:
	PObject();
	virtual ~PObject();

	// Called when the object is spawned into the game
	void Start();
	// Called every frame, passes in the time delta since the last frame
	void Tick(float deltaTime);
	// Called after each Tick in every frame
	void PostTick(float deltaTime);
	// Mark the object for destruction
	void Destroy();
	// Check if the object is marked for destruction
	bool IsPendingDestroy() const { return m_PendingDestroy; }
	// Set the object's lifetime to automatically destroy it after a specified duration
	void SetLifeTime(float lifeTime) {
		m_LifeTime = lifeTime;
		m_LifeTimeTimer = m_LifeTime;
	}

protected:
	// Called when the object is spawned into the game
	virtual void OnStart() {}
	// Called every frame, passes in the time delta since the last frame
	virtual void OnTick(float deltaTime) {}
	// Called after each Tick in every frame
	virtual void OnPostTick(float deltaTime) {}

private:
	// Flag indicating if the object is marked for destruction
	bool m_PendingDestroy;
	// Duration before the object is destroyed, if a lifetime is set
	float m_LifeTime;
	// Timer tracking the remaining lifetime before destruction
	float m_LifeTimeTimer;
};
