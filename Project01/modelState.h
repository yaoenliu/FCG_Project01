#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <unordered_map>
#include <string>

#include "jointState.h"

class modelState
{
public:
	std::unordered_map<std::string, jointState> jointMap;

	jointState& operator[](const std::string& name);
};