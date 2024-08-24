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
		int rowCounter = 0;
		while (*content && rowCounter < sprite.height)
		{
			if (*content == '\n')
			{
				rowCounter += 1;
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

}

