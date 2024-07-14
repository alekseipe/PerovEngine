#pragma once

// External libraries
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

// Structure to represent a transform in 3D space
struct PSTransform
{
	PSTransform()
	{
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}

	// Get the forward vector based on the current rotation
	glm::vec3 Forward()
	{
		glm::vec3 forward;
		forward.x = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		forward.y = sin(glm::radians(rotation.x));
		forward.z = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

		if (glm::length(forward) != 0.0f)
			forward = glm::normalize(forward);

		return forward;
	}

	// Get the right vector based on the current rotation
	glm::vec3 Right()
	{
		glm::vec3 right = glm::cross(Forward(), glm::vec3(0.0f, 1.0f, 0.0f));

		if (glm::length(right) != 0.0f)
			right = glm::normalize(right);

		return right;
	}

	// Get the up vector based on the current rotation
	glm::vec3 Up()
	{
		glm::vec3 up = glm::cross(Right(), Forward());

		if (glm::length(up) != 0.0f)
			up = glm::normalize(up);

		return up;
	}

	glm::vec3 position;  // Position of the transform in 3D space
	glm::vec3 rotation;  // Rotation of the transform in 3D space
	glm::vec3 scale;     // Scale of the transform in 3D space
};
