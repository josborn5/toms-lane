#include "../tl-application.hpp"
#include "../tl-library.hpp"

#include "./game.hpp"
#include "./platform_common.cpp"
#include "./levels.cpp"

tl::bitmap backgroundBitmap;
tl::bitmap playerBitmaps;

#include "./render-gamestate.cpp"
#include "./update-gamestate.cpp"

#include "./assets.hpp"

/*

* checkpoint
* player can land on surface
* levels

*/

tl::GameMemory appMemory;
tl::MemorySpace font_memory;
bool initialized = false;
bool isPaused = false;

int Initialize(const tl::GameMemory& gameMemory)
{
	load_bitmap_from_embed(
		background_bmp,
		background_bmp_len,
		backgroundBitmap
	);

	load_bitmap_from_embed(
		obby2_player_bmp,
		obby2_player_bmp_len,
		playerBitmaps
	);

	return 0;
}

int UpdateAndRender(const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
	tl::Vec2<int> pixelRect;
	pixelRect.x = renderBuffer.width;
	pixelRect.y = renderBuffer.height;

	if (input.buttons[tl::KEY_R].keyUp)
	{
		initialized = false;
	}

	if (!initialized)
	{
		initialized = true;
		return InitializeGameState(&gamestate, pixelRect, input);
	}

	if (input.buttons[tl::KEY_H].keyUp)
	{
		isPaused = !isPaused;
	}

	if (!isPaused)
	{
		UpdateGameState(&gamestate, pixelRect, input, dt, renderBuffer);
	}

	RenderGameState(renderBuffer, gamestate);
	return 0;
}

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return UpdateAndRender(input, renderBuffer, dt);
}

int obby_2_main()
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Obby-2";
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

	tl::font_interface_initialize();

	Initialize(appMemory);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
