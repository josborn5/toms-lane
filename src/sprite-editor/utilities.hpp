#ifndef SPRITE_EDITOR_UTILITIES
#define SPRITE_EDITOR_UTILITIES

int GetSelectedColumnIndex(Grid& grid);
int GetColumnIndex(const Grid& grid, int index);
char* ParseColorFromCharArray(char* content, tl::MemorySpace& space, uint32_t& color);

#endif
