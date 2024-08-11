#pragma once
#include "EngineTypes.h"

// System libraries
#include <functional>

template<typename... Args>
class PEvents
{
public:
	// Add a function to the callbacks array
	PUi8 Bind(const std::function<void(Args...)>& callback)
	{
		// Create a unique pointer for the new callback node
		auto newNode = TMakeUnique<PCallbackNode>();
		// Move the provided callback function into the newly created node
		newNode->callback = std::move(callback);
		// Initialize the callback ID
		PUi8 id = 0;

		if (m_CallbackNodes.size() > 0)
		{
			// Determine a unique ID for the callback node
			// Start with the ID value of 1
			PUi8 potentialID = 1;
			// Loop to find an available ID
			while (id == 0)
			{
				// Assume the ID is available
				bool foundID = false;
				// Check if the ID is already in use by any node
				for (const auto& node : m_CallbackNodes)
				{
					if (node->id == potentialID)
					{
						foundID = true;
						break;
					}
				}
				// If no existing node has the potential ID, assign it to the new node
				if (!foundID)
				{
					id = potentialID;
					break;
				}
				// Increment the potential ID if it was already taken
				++potentialID;
			}
		}
		else
		{
			id = 0;
		}

		newNode->id = id;

		// Add the unique callback node to the array of callbacks
		m_CallbackNodes.push_back(std::move(newNode));

		return id;
	}

	// Execute all functions that have been bound to this event listener
	void Run(const Args... args)
	{
		// Iterate over all stored functions
		for (const auto& node : m_CallbackNodes)
		{
			// Execute each function with the provided arguments
			node->callback(args...);
		}
	}

	// Remove a function based on its index
	// The index is returned by the Bind function
	void Unbind(const PUi8& index)
	{
		std::erase_if(m_CallbackNodes.begin(), m_CallbackNodes.end(),
			[index](const PCallbackNode& node) {
				return node->id == index;
			}
		);
	}

private:
	struct PCallbackNode
	{
		std::function<void(Args...)> callback;
		PUi8 id;
	};

	// Store functions to be executed when the event is triggered
	TArray<TUnique<PCallbackNode>> m_CallbackNodes;
};

typedef PEvents<> PEventsVoid;
