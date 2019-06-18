#pragma once
#include <string>

namespace Tooltip {
	static std::string maxSize = "The size in grid cells of the bounding volume that contains the voxels";
	static std::string rules = "These cryptic values describe the rules of the cellular automaton, they are interpreted as follows:\n\nA live cell must have at least eL and at most eU neighbors to stay alive.\n\nA dead cell must have at least fL and at most fU neighbors to become a live cell.";
	static std::string shaders = "Distance ramp: colors the structure with a gradient based on either the distance from the camera or the distance from the origin of space\n\n Normal / Light: color the structure based on the direction of each face or with a simple directional light";
}