#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

#include "./sprite-operations.cpp"
#include "./sprite-commands.cpp"
#include "./render.cpp"
#include "./sprite-editor-palettes.cpp"
#include "./input-processing.cpp"


EditorState state;

int updateAndRender(const tl::GameMemory& gameMemory, const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
	ProcessActiveControl(input, state);
	ProcessCursorMovementInput(input, state);

	state.mouse.x = input.mouse.x;
	state.mouse.y = input.mouse.y;

	ProcessKeyboardInput(input, state);

	Render(renderBuffer, state);

	return 0;
}

int updateWindowCallback(const tl::Input& input, int dtInMilliseconds, tl::RenderBuffer& renderBuffer)
{
	float dt = (float)dtInMilliseconds / 1000.0f;
	return updateAndRender(appMemory, input, renderBuffer, dt);
}

int tl::main(char* commandLine)
{
	const int targetFPS = 60;

	tl::WindowSettings settings;
	settings.title = "Sprite Editor";
	settings.width = 800;
	settings.height = 600;

	state.windowWidth = 800;
	state.windowHeight = 600;

	if (*commandLine)
	{
		state.filePath = commandLine;
	}

	int windowOpenResult = tl::OpenWindow(settings);
	if (windowOpenResult != 0)
	{
		return windowOpenResult;
	}

	tl::InitializeMemory(
		2,
		2,
		appMemory
	);

	Initialize(appMemory, state);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
