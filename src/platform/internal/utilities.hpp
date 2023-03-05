#ifndef TOMS_LANE_UTILITIES_H
#define TOMS_LANE_UTILITIES_H

#include "./data-structures.hpp"

namespace tl
{
	template<typename T>
	void swap(T& a, T& b)
	{
		T spare;
		spare = a;
		a = b;
		b = spare;
	}

	int CharStringToInt(char* toParse);
}

#endif