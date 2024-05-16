#pragma once

#include "pch.h"

#include "jointState.h"

class modelState
{
public:
	std::unordered_map<std::string, jointState> jointMap;

	jointState& operator[](const std::string& name);
};