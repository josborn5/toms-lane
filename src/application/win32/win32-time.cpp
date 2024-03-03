#include <windows.h>
#include <stdint.h>

namespace tl
{

static int64_t _countsPerSecond;

void win32_time_interface_initialize()
{
	LARGE_INTEGER perfCounterFrequencyResult;
	QueryPerformanceFrequency(&perfCounterFrequencyResult);
	_countsPerSecond = perfCounterFrequencyResult.QuadPart;
}

LARGE_INTEGER win32_time_interface_wallclock_get()
{
	LARGE_INTEGER result;
	QueryPerformanceCounter(&result);
	return result;
}

double win32_time_interface_elapsed_seconds_get(LARGE_INTEGER start)
{
	LARGE_INTEGER end = win32_time_interface_wallclock_get();
	uint64_t counterElapsed = end.QuadPart - start.QuadPart;
	double elapsedSeconds = ((double)counterElapsed / (double)_countsPerSecond);
	return elapsedSeconds;
}

int win32_time_interface_elapsed_microseconds_get(LARGE_INTEGER start)
{
	double secondsElapsed = win32_time_interface_elapsed_seconds_get(start);
	return (int)(secondsElapsed * 1000000.0);
}

}