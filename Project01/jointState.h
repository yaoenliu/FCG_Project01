#pragma once

#include "pch.h"

class jointState
{
public:
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 translation;

	jointState();
	jointState(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) : translation(translation), rotation(rotation), scale(scale) { }

	void operator=(jointState state);
	friend std::fstream& operator>>(std::fstream& fin, jointState& state);
	friend std::fstream& operator<<(std::fstream& fout, const jointState& state);

	glm::mat4 translationMatrix();
	glm::mat4 rotationMatrix();
	glm::mat4 scaleMatrix();
};

