#ifndef TOMS_LANE_FILE_H
#define TOMS_LANE_FILE_H

#include <string>
#include "./geometry.hpp"
#include "./data-structures.hpp"

namespace tl
{
	template<typename T>
	bool ReadObjFileToArray4(std::string const &filename, HeapArray<Triangle4d<T>> &triangles);
}

#endif
