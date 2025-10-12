#ifndef TOMS_LANE_UTILITIES_H
#define TOMS_LANE_UTILITIES_H

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

	MemorySpace CarveMemorySpace(uint64_t carveSizeInBytes, MemorySpace& toCarve);

	int CharStringToInt(char* toParse);
	int IntToCharString(int from, MemorySpace to);
	int IntToCharString(int from, char* to);

	char* CopyToEndOfNumberChar(char* from, char* to);
	char* GetNextNumberChar(char* from);
}

#endif
