static int64_t win32PerformanceCountsPerSecond;

inline void InitializeTime()
{
	LARGE_INTEGER perfCounterFrequencyResult;
	QueryPerformanceFrequency(&perfCounterFrequencyResult);
	win32PerformanceCountsPerSecond = perfCounterFrequencyResult.QuadPart;
}

inline LARGE_INTEGER Win32_GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return Result;
}

inline double Win32_GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
	uint64_t counterElapsed = End.QuadPart - Start.QuadPart;
	double secondsElapsedForWork = ((double)counterElapsed / (double)win32PerformanceCountsPerSecond);
	return secondsElapsedForWork;
}

inline int Win32_GetMicroSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
	double secondsElapsed = Win32_GetSecondsElapsed(start, end);
	return (int)(secondsElapsed * 1000000.0f);
}