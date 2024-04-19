#include "jointState.h"

jointState::jointState()
{
	translation = glm::vec3(0.0f);
	rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void jointState::operator=(jointState state)
{
	translation = state.translation;
	rotation = state.rotation;
	scale = state.scale;
}

std::fstream& operator>>(std::fstream& fin, jointState& state)
{
	std::string str = "";
	std::getline(fin, str);
	sscanf_s(str.c_str(), "%f %f %f", &state.translation.x, &state.translation.y, &state.translation.z);
	std::getline(fin, str);
	sscanf_s(str.c_str(), "%f %f %f %f", &state.rotation.w, &state.rotation.x, &state.rotation.y, &state.rotation.z);
	std::getline(fin, str);
	sscanf_s(str.c_str(), "%f %f %f", &state.scale.x, &state.scale.y, &state.scale.z);

	return fin;
}

std::fstream& operator<<(std::fstream& fout, const jointState& state)
{
	fout << state.translation.x << " " << state.translation.y << " " << state.translation.z << "\n" <<
		state.rotation.w << " " << state.rotation.x << " " << state.rotation.y << " " << state.rotation.z << "\n" <<
		state.scale.x << " " << state.scale.y << " " << state.scale.z << "\n";
	return fout;
}

glm::mat4 jointState::translationMatrix()
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(translation.x * scale.x, translation.y * scale.y, translation.z * scale.z));
}

glm::mat4 jointState::rotationMatrix()
{
	return glm::mat4_cast(rotation);
}

glm::mat4 jointState::scaleMatrix()
{
	return glm::scale(glm::mat4(1.0f), scale);
}

