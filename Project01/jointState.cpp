#include "jointState.h"

std::fstream& operator<<(std::fstream& fout, const jointState& state)
{
	fout << state.translation.x << " " << state.translation.y << " " << state.translation.z << "\n" <<
		state.rotation.w << " " << state.rotation.x << " " << state.rotation.y << " " << state.rotation.z << "\n" <<
		state.scale.x << " " << state.scale.y << " " << state.scale.z << "\n";
	return fout;
}
