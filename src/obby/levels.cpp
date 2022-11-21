#include "./game.hpp"
#include "../platform/toms-lane-platform.hpp"

const float BLOCK_WIDTH = 60.0f;
const float BLOCK_HEIGHT = 30.0f;

char *levels[10] = {
"\
 \n\
 \n\
 \n\
0  0  0  0  0  0  0  0  0\n\
 \n\
 \n\
 \n\
                         ",


"\
     00\n\
    0000\n\
  00000000\n\
CCCCCCCCCCCC",

"\
000000000000\n\
  00C00C00\n\
    0000\n\
     MM"
};
