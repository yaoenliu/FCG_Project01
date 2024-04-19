#include "modelState.h"

jointState& modelState::operator[](const std::string& name)
{
	return jointMap[name];
}
