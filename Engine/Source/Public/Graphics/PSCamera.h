#pragma once
#include "Math/PSTransform.h"

// Structure representing a camera in the 3D space
struct PSCamera
{
	PSCamera()
	{
		fov = 70.0f;
		defaultFov = fov;
		aspectRatio = 1.0f;
		nearClip = 0.01f;
		farClip = 10000.0f;
		moveSpeed = 0.1f;
		rotationSpeed = 1.0f;
	}

	// Rotate the camera based on the given rotation vector
	void Rotate(glm::vec3 rotation, glm::vec3 scale = glm::vec3(1.0f))
	{
		if (glm::length(rotation) != 0.0f)
			rotation = glm::normalize(rotation);

		transform.rotation += rotation * scale * rotationSpeed;

		// Limit the rotation to prevent flipping
		if (transform.rotation.x < -89.9f)
			transform.rotation.x = -89.9f;

		if (transform.rotation.x > 89.9f)
			transform.rotation.x = 89.9f;
	}

	// Translate the camera based on the given translation vector
	void Translate(glm::vec3 translation, glm::vec3 scale = glm::vec3(1.0f))
	{
		glm::vec3 moveDir = transform.Forward() * translation.z;
		moveDir += transform.Right() * translation.x;
		moveDir.y += translation.y;

		if (glm::length(moveDir) != 0.0f)
			moveDir = glm::normalize(moveDir);

		transform.position += moveDir * scale * moveSpeed;
	}

	// Zoom the camera's field of view (FOV) by the given amount
	void Zoom(const float& amount)
	{
		fov -= amount;
	}

	// Reset the camera's FOV to the default value
	void ResetZoom()
	{
		fov = defaultFov;
	}

	// Set a new FOV and update the default FOV
	void SetFOV(const float& newFov)
	{
		fov = newFov;
		defaultFov = newFov;
	}

	// Transform representing the camera's position and orientation
	PSTransform transform;

	// Camera settings
	float fov;              // Field of view
	float defaultFov;       // Default field of view
	float aspectRatio;      // Aspect ratio
	float nearClip;         // Near clipping plane
	float farClip;          // Far clipping plane
	float moveSpeed;        // Speed of movement
	float rotationSpeed;    // Speed of rotation
};
