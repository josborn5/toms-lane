#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./state.hpp"

tl::GameMemory appMemory;

int updateAndRender(const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
	EditorState latestState = GetLatestState(input);
	if (latestState.quit)
	{
		return 1;
	}

	Render(renderBuffer, latestState, dt);

	return 0;
}

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return updateAndRender(input, renderBuffer, dt);
}

int tl::main(char* commandLine)
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Sprite Editor";
	settings.width = 800;
	settings.height = 600;

	int clientX, clientY;
	int windowOpenResult = tl::OpenWindow(settings, clientX, clientY);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::InitializeMemory(
		4,
		1,
		appMemory
	);

	InitializeState(appMemory, commandLine, clientX, clientY);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
