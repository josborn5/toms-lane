#include <stdlib.h>
#include "../../application/src/tl-application.hpp"
#include "../../platform/src/tl-library.hpp"
#include "./editor.hpp"
#include "./render.hpp"
#include "./state.hpp"

tl::MemorySpace persistent_memory;
tl::MemorySpace transient_memory;

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

int sprite_editor_main(char* commandLine)
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

	persistent_memory.sizeInBytes = 8 * 1024 * 1024;
	transient_memory.sizeInBytes = 1 * 1024 * 1024;

	persistent_memory.content = malloc(persistent_memory.sizeInBytes);
	transient_memory.content = malloc(transient_memory.sizeInBytes);

	InitializeState(persistent_memory, transient_memory, commandLine, clientX, clientY);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
