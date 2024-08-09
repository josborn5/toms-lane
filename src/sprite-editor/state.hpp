#ifndef SPRITE_EDITOR_STATE_HPP
#define SPRITE_EDITOR_STATE_HPP

int InitializeState(char* commandLine);
const EditorState& GetLatestState(const tl::Input& input);
void LoadSpriteC(char* content, tl::MemorySpace& space, SpriteC& sprite);

#endif 

