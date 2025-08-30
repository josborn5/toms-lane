#include <windows.h>
#include "../application/win32/win32-application.hpp"
#include "../tl-application.hpp"
#include "../tl-library.hpp"

#include "./driving.hpp"

State gameState = State();

tl::bitmap bitmap;

tl::MemorySpace memory;

static HINSTANCE _instance;

HINSTANCE tl::instance_handle_get() {
	return _instance;
}


int UpdateAndRender(const tl::RenderBuffer& renderBuffer)
{
	for (int i = 0; i < gameState.tileCount; i += 1)
	{
		Tile tile = gameState.tiles[i];
		tl::DrawRect(renderBuffer, tile.color, tile.rect);
	}

	tl::bitmap_interface_render(renderBuffer, bitmap, tl::Vec2<int>{ 0, 0 });

	int render_result = tl::text_interface_render("HELLO!", 160, 40, 160, 40);

	char text[64] = {0};
	tl::IntToCharString(render_result, text);
	tl::console_interface_write(text);

	return 0;
}

int Initialize()
{
	float halfWidth = 10.0f;
	float tileSpacing = halfWidth * 2.0f;
	tl::Vec2<float> halfSize = { halfWidth, 1000.0f };
	tl::Rect<float> baseRect;
	baseRect.halfSize = halfSize;
	baseRect.position = { halfWidth, 0.0f };

	Tile leftGrass = Tile();
	leftGrass.rect = baseRect;
	leftGrass.color = 0x00FF00;
	gameState.tiles[0] = leftGrass;

	Tile road = Tile();
	baseRect.position.x += tileSpacing;
	road.rect = baseRect;
	road.color = 0x111111;
	gameState.tiles[1] = road;

	Tile rightGrass = Tile();
	baseRect.position.x += tileSpacing;
	rightGrass.rect = baseRect;
	rightGrass.color = 0x00FF00;
	gameState.tiles[2] = rightGrass;

	memory.sizeInBytes = 200000;
	tl::memory_interface_initialize(memory);
	tl::file_interface_read("test.bmp", memory);
	tl::bitmap_interface_initialize(bitmap, memory);
	tl::console_interface_open();
	return 0;
}

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	return UpdateAndRender(renderBuffer);
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode) {
	_instance = instance;
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Driving";
	settings.width = 1280;
	settings.height = 720;

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	Initialize();

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
