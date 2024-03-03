#include "../../tl-application.hpp"
#include "../../tl-library.hpp"
#include "./game.hpp"

#include "../platform_common.cpp"

#include "./levels.cpp"
#include "./update-gamestate.cpp"
#include "./render-gamestate.cpp"

/*

* checkpoint
* player can land on surface
* levels

*/

tl::GameMemory appMemory;

bool initialized = false;
bool isPaused = false;
tl::SpriteC regularBlockSprite;
tl::SpriteC checkpointBlockSprite;

int Initialize(const tl::GameMemory& gameMemory)
{
	tl::MemorySpace permanent = gameMemory.permanent;
	tl::MemorySpace transient = gameMemory.transient;

	// Read spritec files
	LoadSpriteFromFile(
		".\\brick.sprc",
		regularBlockSprite,
		permanent,
		transient
	);
	gamestate.blockPixelHalfSize = 2.0f;

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
	gamestate.player.pixelHalfSize = 2.0f;

	return 0;
}

int UpdateAndRender(const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
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

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return UpdateAndRender(input, renderBuffer, dt);
}

int tl::main(char* commandLine)
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Obby-1";
	settings.width = 1280;
	settings.height = 720;

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::InitializeMemory(
		2,
		1,
		appMemory
	);

	Initialize(appMemory);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}

