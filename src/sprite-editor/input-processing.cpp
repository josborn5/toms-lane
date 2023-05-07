#include "../platform/toms-lane-platform.hpp"
#include "./editor.hpp"

static void MoveCursorForSprite(const tl::Input &input, const tl::SpriteC& sprite, int& selectedPixelIndex)
{
	int maxPixelIndex = (sprite.width * sprite.height) - 1;
	if (!input.buttons[tl::KEY_CTRL].isDown)
	{
		if (tl::IsReleased(input, tl::KEY_RIGHT))
		{
			if (selectedPixelIndex < maxPixelIndex)
			{
				selectedPixelIndex += 1;
			}
		}
		else if (tl::IsReleased(input, tl::KEY_LEFT))
		{
			if (selectedPixelIndex > 0)
			{
				selectedPixelIndex -= 1;
			}
		}
		else if (tl::IsReleased(input, tl::KEY_DOWN))
		{
			int provisionalSelectedPixelIndex = selectedPixelIndex + sprite.width;
			if (provisionalSelectedPixelIndex <= maxPixelIndex)
			{
				selectedPixelIndex = provisionalSelectedPixelIndex;
			}
		}
		else if (tl::IsReleased(input, tl::KEY_UP))
		{
			int provisionalSelectedPixelIndex = selectedPixelIndex - sprite.width;
			if (provisionalSelectedPixelIndex >= 0)
			{
				selectedPixelIndex = provisionalSelectedPixelIndex;
			}
		}
	}
}

void ProcessCursorMovement(const tl::Input &input, EditorState& state)
{
	MoveCursorForSprite(input, state.sprite, state.selectedPixelIndex);
}
