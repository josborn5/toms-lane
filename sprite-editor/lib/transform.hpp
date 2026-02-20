#include "../../application/src/tl-application.hpp"
#include "../../platform/lib/tl-library.hpp"
#include "./editor.hpp"

int InitializeBitmapFromSpriteC(
	const SpriteC& sprite,
	tl::bitmap& bitmap,
	const tl::MemorySpace tempMemory);

int InitializeSpriteCFromBitmap(
	SpriteC& sprite,
	const tl::bitmap& bitmap);

