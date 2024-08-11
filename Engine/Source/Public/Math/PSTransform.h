#pragma once

// External Headers
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

struct PSTransform
{
	PSTransform()
	{
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}

	PSTransform(const glm::vec3& p, const glm::vec3& r, const glm::vec3& s)
	{
		position = p;
		rotation = r;
		scale = s;
	}

	// Retrieve the forward vector based on local rotation
	glm::vec3 Forward()
	{
		glm::vec3 forward = glm::vec3(0.0f);

		// Calculate the forward x component using the sine of y and cosine of x
		forward.x = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

		// Calculate the forward y component using the sine of x
		forward.y = sin(glm::radians(rotation.x));

		// Calculate the forward z component using the cosine of y and cosine of x
		forward.z = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

		// Normalize the vector only if it's not zero
		if (glm::length(forward) != 0.0f)
			glm::normalize(forward);

		return forward;
	}

	// Retrieve the right vector by computing the cross product of Forward and World up vector
	glm::vec3 Right()
	{
		glm::vec3 right = glm::cross(Forward(), glm::vec3(0.0f, 1.0f, 0.0f));

		// Normalize the vector only if it's not zero
		if (glm::length(right) != 0.0f)
			glm::normalize(right);

		return right;
	}

	// Retrieve the up vector by computing the cross product of Right and Forward vectors
	glm::vec3 Up()
	{
		glm::vec3 up = glm::cross(Right(), Forward());

		// Normalize the vector only if it's not zero
		if (glm::length(up) != 0.0f)
			glm::normalize(up);

		return up;
	}

	// Operator overload for addition of PSTransform objects
	PSTransform operator+(const PSTransform& other) const
	{
		return
		{
			position + other.position,
			rotation + other.rotation,
			scale + other.scale
		};
	}

	// Operator overload for compound addition assignment of PSTransform objects
	PSTransform& operator+=(const PSTransform& other)
	{
		return *this = *this + other;
	}

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};
