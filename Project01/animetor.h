#pragma once

#include "Object.h"

class keyFrame
{
	Object obj;
	float frameTime;
};

class anime
{
	std::vector<keyFrame> keyFrames;
	float time;
};

