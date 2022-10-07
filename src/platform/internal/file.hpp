#ifndef TOMS_LANE_FILE_H
#define TOMS_LANE_FILE_H

#include <string>
#include <vector>
#include "./geometry.hpp"

namespace tl
{
	template<typename T>
	bool ReadObjFileToVec4(std::string const &filename, std::vector<Triangle4d<T>> &triangles);
}

#endif
