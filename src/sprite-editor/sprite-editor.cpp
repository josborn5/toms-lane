#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./state.hpp"


int updateAndRender(const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
	EditorState latestState = GetLatestState(input);
	Render(renderBuffer, latestState);

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

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	InitializeState(commandLine);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
