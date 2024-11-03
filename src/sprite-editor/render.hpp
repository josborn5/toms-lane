#ifndef SPRITE_EDITOR_RENDER_HPP
#define SPRITE_EDITOR_RENDER_HPP

#include "../tl-library.hpp"

void InitializeLayout(EditorState& state);
void Render(const tl::RenderBuffer& renderBuffer, const EditorState& latestState, float dt);

#endif

