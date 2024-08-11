#pragma once
#include "Math/PSTransform.h"

struct PSCamera
{
	PSCamera()
	{
		fov = 70.0f;
		defaultFov = fov;
		aspectRatio = 1.0f;
		nearClip = 0.01f;
		farClip = 10000.0f;
		moveSpeed = 4.0f;
		rotationSpeed = 1.5f;
	}

	// Rotate the camera based on the provided rotation vector
	void Rotate(glm::vec3 rotation, glm::vec3 scale = glm::vec3(1.0f))
	{
		if (glm::length(rotation) != 0.0f)
			rotation = glm::normalize(rotation);

		transform.rotation += rotation * scale * rotationSpeed;

		if (transform.rotation.x < -89.9f)
			transform.rotation.x = -89.9f;

		if (transform.rotation.x > 89.9f)
			transform.rotation.x = 89.9f;
	}

	// Translate the camera based on the provided translation vector
	void Translate(glm::vec3 translation, glm::vec3 scale = glm::vec3(1.0f))
	{
		// Calculate the movement direction, adjusting for forward and right vectors
		glm::vec3 moveDir = transform.Forward() * translation.z;
		moveDir += transform.Right() * translation.x;
		moveDir.y += translation.y;

		if (glm::length(moveDir) != 0.0f)
			moveDir = glm::normalize(moveDir);

		transform.position += moveDir * scale * moveSpeed;
	}

	// Adjust the field of view (FOV) by the specified amount
	void Zoom(const float& amount)
	{
		fov -= amount;
	}

	// Reset the FOV to its default value
	void ResetZoom()
	{
		fov = defaultFov;
	}

	// Set a new FOV and update the default FOV to match
	void SetFOV(const float& newFov)
	{
		fov = newFov;
		defaultFov = fov;
	}

	PSTransform transform;
	float fov;
	float defaultFov; // Initial FOV value, automatically set during initialization
	float aspectRatio;
	float nearClip;
	float farClip;
	float moveSpeed;
	float rotationSpeed;
};
