#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <vector>

class modelState
{
public:
	bool isJoint;
	glm::vec3 scale;
	glm::fquat rotation;
	glm::vec3 translation;
	std::vector<modelState> children;

	modelState()
	{
		isJoint = 0;
		translation = glm::vec3(0.0f);
		rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}
};