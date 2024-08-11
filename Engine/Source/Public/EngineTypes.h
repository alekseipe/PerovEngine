#pragma once
#include "Debug/PDebug.h"

// Standard Library Inclusions
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

// Custom definition for the standard string type
typedef std::string PString;

// Custom definitions for unsigned integer types
typedef uint8_t PUi8;
typedef uint16_t PUi16;
typedef uint32_t PUi32;
typedef uint64_t PUi64;

// Custom definition for the standard vector template
template <typename T>
using TArray = std::vector<T>;

// Custom definitions for smart pointers
// Custom definition for a shared pointer
template <typename T>
using TShared = std::shared_ptr<T>;
// Custom definition for a unique pointer
template <typename T>
using TUnique = std::unique_ptr<T>;
// Custom definition for a weak pointer
template <typename T>
using TWeak = std::weak_ptr<T>;

// Custom definition for the make_shared utility
template <typename T, typename... Args> // ... = expecting additional arguments
TShared<T> TMakeShared(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

// Custom definition for the make_unique utility
template <typename T, typename... Args>
TUnique<T> TMakeUnique(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}
