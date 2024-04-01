#ifndef SPRITE_EDITOR_RENDER_HPP
#define SPRITE_EDITOR_RENDER_HPP

#include "../tl-library.hpp"

void InitializeLayout(const EditorState& state);
void SizePalette(const tl::SpriteC& palette);
void SizeGridForSprite(const tl::SpriteC& sprite);
void Render(const tl::RenderBuffer& renderBuffer, const EditorState& latestState);

#endif

