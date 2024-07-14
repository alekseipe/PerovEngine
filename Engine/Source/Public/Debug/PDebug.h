#pragma once
#include <cstdint>
#include <iostream>
#include <string>

// Log type enumeration
enum PELogType : uint8_t
{
	LT_LOG = 0U,    // Standard log message
	LT_WARN,        // Warning message
	LT_ERROR,       // Error message
	LT_SUCCESS      // Success message
};

// Debug class for logging messages to the console
class PDebug
{
public:
	// Log a message to the console based on the log type
	static void Log(const std::string& message, const PELogType& logType = LT_LOG)
	{
		std::cout << message << std::endl;
	}
};
