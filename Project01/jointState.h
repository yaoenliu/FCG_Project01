#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <fstream>
#include <iostream>
#include <string>

class jointState
{
public:
	glm::vec3 scale;
	glm::fquat rotation;
	glm::vec3 translation;

	jointState();
	jointState(glm::vec3 translation, glm::fquat rotation, glm::vec3 scale) : translation(translation), rotation(rotation), scale(scale) { }

	void operator=(jointState state);
	friend std::fstream& operator>>(std::fstream& fin, jointState& state);
	friend std::fstream& operator<<(std::fstream& fout, const jointState& state);

	glm::mat4 translationMatrix();
	glm::mat4 rotationMatrix();
	glm::mat4 scaleMatrix();
};

