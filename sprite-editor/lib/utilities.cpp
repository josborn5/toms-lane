#include "./editor.hpp"
#include "./utils.hpp"
#include <stdlib.h>

const char* ParseColorFromCharArray(const char* content, tl::MemorySpace& space, uint32_t& color)
{
	char* buffer = (char*)space.content;
	const char* workingPointer = content;

	/// RBGA values
	int rgbContent[3] = { 0, 0, 0 };

	for (int i = 0; i < 3 && *workingPointer; i += 1)
	{
		workingPointer = GetNextNumberChar(workingPointer);
		if (*workingPointer)
		{
			workingPointer = CopyToEndOfNumberChar(workingPointer, buffer);
			rgbContent[i] = atoi(buffer);
		}
	}

	color = tl::GetColorFromRGB(rgbContent[0], rgbContent[1], rgbContent[2]);
	return workingPointer;
}

