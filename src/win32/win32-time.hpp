namespace tl
{

typedef struct Win32Time Win32Time;

Win32Time win32_time_interface_initialize();

LARGE_INTEGER win32_time_interface_wallclock_get(const Win32TimeTemp& time);

double win32_time_interface_elapsed_seconds_get(const Win32TimeTemp& time, LARGE_INTEGER start);

int win32_time_interface_elapsed_microseconds_get(const Win32TimeTemp& time, LARGE_INTEGER start);

}
