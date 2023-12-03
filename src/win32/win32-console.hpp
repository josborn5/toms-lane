#ifndef TOMS_LANE_WIN32_CONSOLE
#define TOMS_LANE_WIN32_CONSOLE

namespace tl
{

int win32_console_interface_open();

int win32_console_interface_write(char* message);

}

#endif