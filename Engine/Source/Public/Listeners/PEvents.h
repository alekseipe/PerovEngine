#pragma once
#include "EngineTypes.h"

// System libraries
#include <functional>
#include <algorithm>

// Template class for event handling
template<typename... Args>
class PEvents
{
public:
	// Add a function to the callbacks array
	PUi8 Bind(const std::function<void(Args...)>& callback)
	{
		// Create a unique pointer for the new callback node
		auto newNode = TMakeUnique<PCallbackNode>();
		newNode->callback = std::move(callback);
		PUi8 id = 1;

		if (!m_CallbackNodes.empty())
		{
			// Find a unique ID for the callback node
			while (std::any_of(m_CallbackNodes.begin(), m_CallbackNodes.end(),
				[id](const TUnique<PCallbackNode>& node) { return node->id == id; }))
			{
				++id;
			}
		}

		newNode->id = id;
		// Add the unique callback node to the array
		m_CallbackNodes.push_back(std::move(newNode));

		return id;
	}

	// Run all functions bound to this event listener
	void Run(const Args... args)
	{
		// Loop through all stored functions and run them with the provided arguments
		for (const auto& node : m_CallbackNodes)
		{
			node->callback(args...);
		}
	}

	// Unbind a function based on the index
	void Unbind(const PUi8& index)
	{
		std::erase_if(m_CallbackNodes, [index](const TUnique<PCallbackNode>& node) {
			return node->id == index;
			});
	}

private:
	// Structure for storing callback information
	struct PCallbackNode
	{
		std::function<void(Args...)> callback; // The callback function
		PUi8 id;                              // The unique ID for the callback
	};

	// Array for storing callback nodes
	TArray<TUnique<PCallbackNode>> m_CallbackNodes;
};

// Specialization for events with no arguments
typedef PEvents<> PEventsVoid;
