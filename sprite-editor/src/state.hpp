#ifndef SPRITE_EDITOR_STATE_HPP
#define SPRITE_EDITOR_STATE_HPP

int InitializeState(
	const tl::MemorySpace& persistent,
	const tl::MemorySpace& transient,
	char* commandLine, int clientX, int clientY);
const EditorState& GetLatestState(const tl::Input& input);

#endif 

