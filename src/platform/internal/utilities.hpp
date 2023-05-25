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

	char* CopyToChar(char* from, char* to, const char toChar);
	char* CopyToEndOfNumberChar(char* from, char* to);
	char* GetNextNumberChar(char* from);

	template<typename T>
	int DeleteFromArray(T* content, unsigned int inclusiveStartIndex, unsigned int inclusiveEndIndex, unsigned int length)
	{
		if (inclusiveStartIndex >= length ||
			inclusiveEndIndex >= length ||
			inclusiveStartIndex > inclusiveEndIndex)
		{
			return -1;
		}

		unsigned int startCopyIndex = inclusiveEndIndex + 1;
		unsigned int offset = startCopyIndex - inclusiveStartIndex;
		for (unsigned int sourceIndex = startCopyIndex; sourceIndex < length; sourceIndex += 1)
		{
			unsigned int targetIndex = sourceIndex - offset;
			T copyValue = content[sourceIndex];
			content[targetIndex] = copyValue;
		}

		unsigned int subtractionCount = inclusiveEndIndex - inclusiveStartIndex + 1;
		return subtractionCount;
	}
}

#endif