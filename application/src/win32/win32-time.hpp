#ifndef TOMS_LANE_WIN32_TIME
#define TOMS_LANE_WIN32_TIME

namespace tl
{

void win32_time_interface_initialize();

LARGE_INTEGER win32_time_interface_wallclock_get();

double win32_time_interface_elapsed_seconds_get(LARGE_INTEGER start);

int win32_time_interface_elapsed_microseconds_get(LARGE_INTEGER start);

}

#endif