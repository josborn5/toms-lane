#include "./software-rendering.hpp"
#include "./utilities.hpp"

namespace tl
{
	tl::Vec2<int> GetContentDimensions(char* content)
	{
		int height = 0;
		int width = 0;
		int rowCounter = 0;
		while (*content)
		{
			if (rowCounter == 0 && height == 0)
			{
				height += 1;
			}
			if (*content == '\n')
			{
				if (width < rowCounter)
				{
					width = rowCounter;
				}
				rowCounter = 0;
				height += 1;
			}
			else
			{
				rowCounter += 1;
			}
			content++;
		}

		// Check the final row (it may not end in a \n char)
		if (width < rowCounter)
		{
			width = rowCounter;
		}

		// Set the width value if no newline char exists in the content
		if (height == 1)
		{
			width = rowCounter;
		}
		tl::Vec2<int> dim = { width, height };
		return dim;
	}

	Sprite LoadSprite(char* content)
	{
		Sprite sprite = Sprite();
		sprite.content = content;
		tl::Vec2<int> dimensions = GetContentDimensions(content);
		sprite.height = dimensions.y;
		sprite.width = dimensions.x;
		return sprite;
	}

	void DrawSprite(
		const RenderBuffer &renderBuffer,
		const Sprite &sprite,
		const Rect<float> &footprint,
		uint32_t color
	) {
		// Work out the size of each block in the sprite
		float footprintHeight = footprint.halfSize.y * 2.0f;
		float footprintWidth = footprint.halfSize.x * 2.0f;

		float blockWidth = footprintWidth / (float)sprite.width;
		float blockHeight = footprintHeight / (float)sprite.height;
		Vec2<float> blockHalf = { 0.5f * blockWidth, 0.5f * blockHeight };

		// Calculate the cursor area over which to position each block. Position is measured fro the center
		// so apply an offset for the block size
		Vec2<float> pCopy = Vec2<float>
		{
			footprint.position.x - footprint.halfSize.x + blockHalf.x,
			footprint.position.y + footprint.halfSize.y - blockHalf.y
		};

		// iterate through the sprite content and fill blocks in the render buffer
		float xMinCursorPos = pCopy.x;
		char* content = sprite.content;
		while (*content)
		{
			if (*content == '\n')
			{
				pCopy.y -= blockHeight;	// We're populating blocks in the sprint left to right, top to bottom. So y is decreasing.
				pCopy.x = xMinCursorPos; // reset cursor to start of next row
			}
			else
			{
				if (*content != ' ')
				{
					Rect<float> blockRect;
					blockRect.position = pCopy;
					blockRect.halfSize = blockHalf;
					DrawRect(renderBuffer, color, blockRect);
				}
				pCopy.x += blockWidth;
			}
			content++;
		}
	}


	char* ParseColorFromCharArray(char* content, MemorySpace& space, Color& color)
	{
		char* buffer = (char*)space.content;
		char* workingPointer = content;

		/// RBGA values
		int rgbaContent[4] = { 0, 0, 0, 255 }; // Default alpha to 100%

		for (int i = 0; i < 4 && *workingPointer; i += 1)
		{
			workingPointer = GetNextNumberChar(workingPointer);
			if (*workingPointer)
			{
				workingPointer = CopyToEndOfNumberChar(workingPointer, buffer);
				rgbaContent[i] = CharStringToInt(buffer);
			}
		}

		color.r = (float)rgbaContent[0] / 255.0f;
		color.g = (float)rgbaContent[1] / 255.0f;
		color.b = (float)rgbaContent[2] / 255.0f;
		color.a = (float)rgbaContent[3] / 255.0f;

		return workingPointer;
	}

	uint64_t GetSpriteSpaceInBytes(const SpriteC& sprite)
	{
		int pixelCount = sprite.width * sprite.height;
		return sizeof(Color) * pixelCount;
	}


	/*
	* Assumed char* format is:
	* width<int>\n
	* height<int>\n
	* RValue<char>, GValue<char>, BValue<char>, AValue<char>\n // 1st pixel
	* :
	* RValue<char>, GValue<char>, BValue<char>, AValue<char>\n // Nth pixel
	*/
	void LoadSpriteC(char* content, MemorySpace& space, SpriteC& sprite)
	{
		char* buffer = (char*)space.content;
		// Width
		char* workingPointer = GetNextNumberChar(content);
		workingPointer = CopyToEndOfNumberChar(workingPointer, buffer);
		int width = CharStringToInt(buffer);

		// Height
		workingPointer = GetNextNumberChar(workingPointer);
		workingPointer = CopyToEndOfNumberChar(workingPointer, buffer);
		int height = CharStringToInt(buffer);

		// Content
		int contentCount = height * width;

		sprite.width = width;
		sprite.height = height;

		for (int i = 0; i < contentCount && *workingPointer; i += 1)
		{
			workingPointer = ParseColorFromCharArray(workingPointer, space, sprite.content[i]);
		}
	}

}

