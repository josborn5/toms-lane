inline LARGE_INTEGER Win32_GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return Result;
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

