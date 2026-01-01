#ifndef SPRITE_EDITOR_STATE_HPP
#define SPRITE_EDITOR_STATE_HPP

int InitializeState(const tl::GameMemory& gameMemory, char* commandLine, int clientX, int clientY);
const EditorState& GetLatestState(const tl::Input& input);

#endif 

