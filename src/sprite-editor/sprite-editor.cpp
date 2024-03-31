#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

#include "./sprite-operations.cpp"
#include "./sprite-commands.cpp"
#include "./render.cpp"
#include "./sprite-editor-palettes.cpp"
#include "./input-processing.cpp"

#define COMMAND_BUFFER_SIZE 15
#define DISPLAY_BUFFER_SIZE 15

tl::GameMemory appMemory;

static char commandBuffer[COMMAND_BUFFER_SIZE];
static char displayBuffer[DISPLAY_BUFFER_SIZE];
tl::array<char> commands = tl::array<char>(commandBuffer, COMMAND_BUFFER_SIZE);
tl::array<char> display = tl::array<char>(displayBuffer, DISPLAY_BUFFER_SIZE);

EditorState state;


static tl::MemorySpace spriteMemory;
static tl::MemorySpace paletteMemory;

int Initialize(const tl::GameMemory& gameMemory)
{
	state.commandBuffer = &commands;
	state.displayBuffer = &display;

	// Define memory slices
	tl::MemorySpace perm = gameMemory.permanent;
	const uint64_t oneKiloByteInBytes = 1024;
	const uint64_t oneMegaByteInBytes = oneKiloByteInBytes * 1024;
	paletteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, perm);
	spriteMemory = tl::CarveMemorySpace(oneMegaByteInBytes, perm);
	tl::MemorySpace temp = gameMemory.transient;
	tl::MemorySpace fileReadMemory = tl::CarveMemorySpace(oneMegaByteInBytes, temp);
	tl::MemorySpace tempMemory = tl::CarveMemorySpace(oneMegaByteInBytes, temp);

	// Load file
	if (state.filePath)
	{
		uint64_t fileSize = 0;
		if (tl::file_interface_size_get(state.filePath, fileSize) != tl::Success)
		{
			return 1;
		}

		if (tl::file_interface_read(state.filePath, fileReadMemory) != tl::Success)
		{
			return 1;
		}
	}
	else
	{
		// Initialize default sprite
		fileReadMemory.content = "2\n2\n0 0 0 0\n0 0 0 0\n0 0 0 0\n0 0 0 0";
	}

	ClearCommandBuffer(state);
	ClearDisplayBuffer(state);

	InitializeLayout(state);
	InitializePalettes(paletteMemory, tempMemory, state);

	char* spriteCharArray = (char*)fileReadMemory.content;
	state.sprite.content = (tl::Color*)spriteMemory.content;
	tl::LoadSpriteC(spriteCharArray, tempMemory, state.sprite);

	SizeGridForSprite(state.sprite);
	return 0;
}

int updateAndRender(const tl::GameMemory& gameMemory, const tl::Input& input, const tl::RenderBuffer& renderBuffer, float dt)
{
	ProcessActiveControl(input, state);
	ProcessCursorMovementInput(input, state);

	state.mouse.x = input.mouse.x;
	state.mouse.y = input.mouse.y;

	ProcessKeyboardInput(input, state, gameMemory, spriteMemory);

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

	Initialize(appMemory);

	return tl::RunWindowUpdateLoop(targetFPS, &updateWindowCallback);
}
