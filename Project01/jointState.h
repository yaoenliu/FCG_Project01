#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>

class jointState
{
public:
	glm::vec3 scale;
	glm::fquat rotation;
	glm::vec3 translation;

	jointState()
	{
		translation = glm::vec3(0.0f);
		rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	jointState(glm::vec3 translation, glm::fquat rotation, glm::vec3 scale) : translation(translation), rotation(rotation), scale(scale) 
	{

	}

	void operator=(jointState state)
	{
		translation = state.translation;
		rotation = state.rotation;
		scale = state.scale;
	}

	glm::mat4 translationMatrix()
	{
		return glm::translate(glm::mat4(1.0f), glm::vec3(translation.x * scale.x, translation.y * scale.y, translation.z * scale.z));
	}

	glm::mat4 rotationMatrix()
	{
		return glm::mat4_cast(rotation);
	}

	glm::mat4 scaleMatrix()
	{
		return glm::scale(glm::mat4(1.0f), scale);
	}
};

