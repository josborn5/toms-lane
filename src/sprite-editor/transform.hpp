#include "../tl-application.hpp"
#include "../tl-library.hpp"
#include "./editor.hpp"

int InitializeBitmapFromSpriteC(
	const SpriteC& sprite,
	tl::bitmap& bitmap,
	const tl::MemorySpace tempMemory);

int InitializeSpriteCFromBitmap(
	SpriteC& sprite,
	const tl::bitmap& bitmap,
	const tl::MemorySpace tempMemory);

