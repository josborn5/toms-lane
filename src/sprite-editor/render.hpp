#ifndef SPRITE_EDITOR_RENDER_HPP
#define SPRITE_EDITOR_RENDER_HPP

#include "../tl-library.hpp"

void InitializeLayout(EditorState& state);
void SizeGrid(Grid& grid);
void Render(const tl::RenderBuffer& renderBuffer, const EditorState& latestState);

#endif

