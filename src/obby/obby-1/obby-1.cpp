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

static int LoadSpriteFromFile(
	char* fileName,
	tl::SpriteC& spriteTarget,
	tl::MemorySpace& permanent,
	tl::MemorySpace transient // Purposefully don't pass as a reference so as not to modify the transient space - it can be overwritten after the function call
) {
	uint64_t fileSize = 0;
	tl::GetFileSize(fileName, fileSize);
	if (tl::ReadFile(fileName, transient) != tl::Success)
	{
		return 1;
	}
	tl::MemorySpace tempFileContentMemory = tl::CarveMemorySpace(fileSize, transient);

	// Generate SpriteCs in perm space
	char* spriteCharArray = (char*)tempFileContentMemory.content;
	spriteTarget.content = (tl::Color*)permanent.content;
	tl::LoadSpriteC(spriteCharArray, transient, spriteTarget);
	tl::CarveMemorySpace(GetSpriteSpaceInBytes(spriteTarget), permanent);

	return 0;
}

int tl::Initialize(const GameMemory &gameMemory, const RenderBuffer &renderBuffer)
{
	tl::MemorySpace permanent = gameMemory.permanent;
	tl::MemorySpace transient = gameMemory.transient;

	// Read spritec files
	LoadSpriteFromFile(
		"brick.sprc",
		regularBlockSprite,
		permanent,
		transient
	);

	LoadSpriteFromFile(
		"checkpoint.sprc",
		checkpointBlockSprite,
		permanent,
		transient
	);

	LoadSpriteFromFile(
		"player.sprc",
		gamestate.player.spriteTest,
		permanent,
		transient
	);

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

