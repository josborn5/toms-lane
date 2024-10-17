#ifndef SPRITE_EDITOR_UTILITIES
#define SPRITE_EDITOR_UTILITIES

int GetSelectedColumnIndex(Grid& grid);
int GetSelectedRowIndex(const Grid& grid);
int GetRowIndex(const Grid& grid, int index);
int GetColumnIndex(const Grid& grid, int index);
char* ParseColorFromCharArray(char* content, tl::MemorySpace& space, Color& color);

#endif
