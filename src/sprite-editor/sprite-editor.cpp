#include "../platform/toms-lane-platform.hpp"
#include "../win32/toms-lane-win32.hpp"
#include "./sprite-editor-win32.cpp"

tl::SpriteC* sprite;
tl::Rect<float> rootGrid;

int tl::Initialize(const GameMemory& gameMemory, const RenderBuffer& renderBuffer)
{
	// Load file
	tl::MemorySpace fileReadMemory;
	fileReadMemory.sizeInBytes = gameMemory.PermanentStorageSpace;
	fileReadMemory.content = gameMemory.PermanentStorage;

	uint64_t fileSize = 0;
	if (tl::GetFileSize(filePath, fileSize) != tl::Success)
	{
		return 1;
	}

	if (tl::ReadFile(filePath, fileReadMemory) != tl::Success)
	{
		return 1;
	}

	sprite = (tl::SpriteC*)fileReadMemory.content;

	float aspectRatio = (float)sprite->height / (float)sprite->width;
	if (aspectRatio >= 1)
	{
		rootGrid.halfSize.y = 600.0f * 0.5f;
		rootGrid.halfSize.x = aspectRatio * rootGrid.halfSize.y * 0.5f;
	}
	else
	{
		rootGrid.halfSize.x = 800;
		rootGrid.halfSize.y = aspectRatio * rootGrid.halfSize.x * 0.5f;
	}
	rootGrid.position = tl::CopyVec2(rootGrid.halfSize);

	return 0;
}

int tl::UpdateAndRender(const GameMemory &gameMemory, const Input &input, const RenderBuffer &renderBuffer, float dt)
{
	tl::DrawRect(renderBuffer, 0xFF0000, rootGrid);
	return 0;
}
