#include "./software-rendering.hpp"
#include "./utilities.hpp"

namespace tl
{
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

	void DrawSpriteC(
		const RenderBuffer &renderBuffer,
		const SpriteC &sprite,
		const Rect<float> &footprint
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
		Color* content = sprite.content;
		int contentLength = sprite.height * sprite.width;
		int rowCounter = 0;
		for (int i = 0; i < contentLength; i += 1)
		{
			Color blockColor = content[i];

			// TODO: handle the color.a value (0.0f means transparent)
			if (blockColor.a > 0.0f)
			{
				Rect<float> blockRect;
				blockRect.position = pCopy;
				blockRect.halfSize = blockHalf;
				uint32_t color = GetColorFromRGB(
					(int)(255.0f * blockColor.r),
					(int)(255.0f * blockColor.g),
					(int)(255.0f * blockColor.b)
				);
				DrawRect(renderBuffer, color, blockRect);
			}

			rowCounter += 1;
			if (rowCounter >= sprite.width)
			{
				rowCounter = 0;
				pCopy.y -= blockHeight;	// We're populating blocks in the sprint left to right, top to bottom. So y is decreasing.
				pCopy.x = xMinCursorPos; // reset cursor to start of next row
			}
			else
			{
				pCopy.x += blockWidth;
			}
		}
	}
}

