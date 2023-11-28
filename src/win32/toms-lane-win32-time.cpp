inline LARGE_INTEGER Win32_GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return Result;
}

struct Win32TimeTemp
{
	int64_t _countsPerSecond;
};

Win32TimeTemp win32_time_interface_initialize()
{
	LARGE_INTEGER perfCounterFrequencyResult;
	QueryPerformanceFrequency(&perfCounterFrequencyResult);
	Win32TimeTemp time;
	time._countsPerSecond = perfCounterFrequencyResult.QuadPart;
	return time;
}

LARGE_INTEGER win32_time_interface_wallclock_get(const Win32TimeTemp& time)
{
	LARGE_INTEGER result;
	QueryPerformanceCounter(&result);
	return result;
}

double win32_time_interface_elapsed_seconds_get(const Win32TimeTemp& time, LARGE_INTEGER start)
{
	LARGE_INTEGER end = Win32_GetWallClock();
	uint64_t counterElapsed = end.QuadPart - start.QuadPart;
	double elapsedSeconds = ((double)counterElapsed / (double)time._countsPerSecond);
	return elapsedSeconds;
}

int win32_time_interface_elapsed_microseconds_get(const Win32TimeTemp& time, LARGE_INTEGER start)
{
	double secondsElapsed = win32_time_interface_elapsed_seconds_get(time, start);
	return (int)(secondsElapsed * 1000000.0);
}

class Win32Time
{
public:
	void initialize()
	{
		LARGE_INTEGER perfCounterFrequencyResult;
		QueryPerformanceFrequency(&perfCounterFrequencyResult);
		_performanceCountsPerSecond = perfCounterFrequencyResult.QuadPart;
	}

	double getSecondsElapsed(LARGE_INTEGER start) const
	{
		LARGE_INTEGER end = Win32_GetWallClock();
		uint64_t counterElapsed = end.QuadPart - start.QuadPart;
		double secondsElapsedForWork = ((double)counterElapsed / (double)_performanceCountsPerSecond);
		return secondsElapsedForWork;
	}

	int getMicroSecondsElapsed(LARGE_INTEGER start) const
	{
		double secondsElapsed = getSecondsElapsed(start);
		return (int)(secondsElapsed * 1000000.0);
	}

private:
	int64_t _performanceCountsPerSecond;
};

