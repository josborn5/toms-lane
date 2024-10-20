#include "./editor.hpp"

char* ParseColorFromCharArray(char* content, tl::MemorySpace& space, uint32_t& color)
{
	char* buffer = (char*)space.content;
	char* workingPointer = content;

	/// RBGA values
	int rgbContent[3] = { 0, 0, 0 };

	for (int i = 0; i < 3 && *workingPointer; i += 1)
	{
		workingPointer = tl::GetNextNumberChar(workingPointer);
		if (*workingPointer)
		{
			workingPointer = tl::CopyToEndOfNumberChar(workingPointer, buffer);
			rgbContent[i] = tl::CharStringToInt(buffer);
		}
	}

	color = tl::GetColorFromRGB(rgbContent[0], rgbContent[1], rgbContent[2]);
	return workingPointer;
}

