#include "../game.hpp"
#include "../../platform/toms-lane-platform.hpp"

#include "../obby-win32.cpp"
#include "../platform_common.cpp"

#include "./levels.cpp"
#include "./update-gamestate.cpp"
#include "./render-gamestate.cpp"

/*

* checkpoint
* player can land on surface
* levels

*/

bool initialized = false;
bool isPaused = false;
tl::SpriteC regularBlockSprite;
tl::SpriteC checkpointBlockSprite;

tl::Color testSpriteContent[4] = {
	{ 1.0f, 0.0f, 0.0f, 1.0f },  { 0.0f, 1.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f, 1.0f },  { 1.0f, 0.0f, 1.0f, 1.0f }
};

int tl::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{
	gamestate.player.spriteTest.height = 2;
	gamestate.player.spriteTest.width = 2;
	gamestate.player.spriteTest.content = testSpriteContent;

	tl::MemorySpace permanent = gameMemory.permanent;
	tl::MemorySpace transient = gameMemory.transient;

	// Read spritec files to temp space
	uint64_t fileSize = 0;
	tl::GetFileSize("brick.sprc", fileSize);
	if (tl::ReadFile("brick.sprc", transient) != tl::Success)
	{
		return 1;
	}

	tl::MemorySpace tempMemory = CarveMemorySpace(fileSize, transient);

	// Generate SpriteCs in perm space
	char* spriteCharArray = (char*)transient.content;
	regularBlockSprite.content = (tl::Color*)permanent.content;
	tl::LoadSpriteC(spriteCharArray, tempMemory, regularBlockSprite);

	tl::GetFileSize("checkpoint.sprc", fileSize);
	if (tl::ReadFile("checkpoint.sprc", transient) != tl::Success)
	{
		return 1;
	}
	spriteCharArray = (char*)transient.content;
	checkpointBlockSprite.content = regularBlockSprite.content + GetSpriteSpaceInBytes(regularBlockSprite);
	tl::LoadSpriteC(spriteCharArray, tempMemory, checkpointBlockSprite);

	return 0;
}

int tl::UpdateAndRender(const GameMemory &gameMemory, const tl::Input &input, const RenderBuffer &renderBuffer, float dt)
{
	tl::Vec2<int> pixelRect;
	pixelRect.x = renderBuffer.width;
	pixelRect.y = renderBuffer.height;

	if (tl::IsReleased(input, tl::KEY_R))
	{
		initialized = false;
	}

	if (!initialized)
	{
		initialized = true;
		return InitializeGameState(&gamestate, pixelRect, input);
	}

	if (tl::IsReleased(input, tl::KEY_H))
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		// player
		tl::DrawSprite(renderBuffer, islaSprite, gamestate.player, 0x154DDA);
		
		UpdateGameState(&gamestate, pixelRect, input, dt, renderBuffer);
	}

	RenderGameState(renderBuffer, gamestate, regularBlockSprite, checkpointBlockSprite);

	float fontSize = 16.0f;
	float infoHeight = 4.0f * fontSize;
	tl::Rect<float> charFoot;
	charFoot.position = { 200.0f, infoHeight };
	charFoot.halfSize = { 4.0f, 0.4f * fontSize };

	tl::DrawAlphabetCharacters(renderBuffer, "SPACE", charFoot, 0x999999);
	charFoot.position.y -= fontSize;
	int spaceIsDown = input.buttons[tl::KEY_SPACE].isDown ? 1 : 0;
	tl::DrawNumber(renderBuffer, spaceIsDown, charFoot, 0x999999);
	charFoot.position.y -= fontSize;
	int spaceWasDown = input.buttons[tl::KEY_SPACE].wasDown ? 1 : 0;
	tl::DrawNumber(renderBuffer, spaceWasDown, charFoot, 0x999999);

	return 0;
}

