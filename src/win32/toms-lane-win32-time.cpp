inline LARGE_INTEGER Win32_GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return Result;
}

struct Win32TimeTemp
{
	int64_t _performanceCountsPerSecond;
};

Win32TimeTemp win32_time_interface_initialize()
{
	LARGE_INTEGER perfCounterFrequencyResult;
	QueryPerformanceFrequency(&perfCounterFrequencyResult);
	Win32TimeTemp timeTemp;
	timeTemp._performanceCountsPerSecond = perfCounterFrequencyResult.QuadPart;
	return timeTemp;
}

double win32_time_interface_secondselapsed_get(const Win32TimeTemp& time, LARGE_INTEGER Start, LARGE_INTEGER End)
{
	uint64_t counterElapsed = End.QuadPart - Start.QuadPart;
	double secondsElapsedForWork = ((double)counterElapsed / (double)time._performanceCountsPerSecond);
	return secondsElapsedForWork;
}

int win32_time_interface_microsecondselapsed_get(const Win32TimeTemp& time, LARGE_INTEGER start)
{
	LARGE_INTEGER end = Win32_GetWallClock();
	double secondsElapsed = win32_time_interface_secondselapsed_get(time, start, end);
	return (int)(secondsElapsed * 1000000.0f);
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

	double getSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End) const
	{
		uint64_t counterElapsed = End.QuadPart - Start.QuadPart;
		double secondsElapsedForWork = ((double)counterElapsed / (double)_performanceCountsPerSecond);
		return secondsElapsedForWork;
	}

	int getMicroSecondsElapsed(LARGE_INTEGER start) const
	{
		LARGE_INTEGER end = Win32_GetWallClock();
		double secondsElapsed = getSecondsElapsed(start, end);
		return (int)(secondsElapsed * 1000000.0f);
	}

private:
	int64_t _performanceCountsPerSecond;
};

