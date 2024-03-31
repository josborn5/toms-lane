#ifndef SPRITE_EDITOR_STATE_HPP
#define SPRITE_EDITOR_STATE_HPP

int InitializeState(char* commandLine);
const EditorState& GetLatestState(const tl::Input& input);
void Render(const tl::RenderBuffer& renderBuffer, const EditorState& latestState);

#endif 

