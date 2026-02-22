namespace tl
{
	MemorySpace CarveMemorySpace(uint64_t carveSizeInBytes, MemorySpace& toCarve)
	{
		// This slices from the front of the address space.
		// Not sure if it's better to slice from the front or the back :shrug:
		MemorySpace carvedSlice;
		carvedSlice.content = toCarve.content;
		
		uint64_t remainingSpace = toCarve.sizeInBytes - carveSizeInBytes;
		if (remainingSpace < 0)
		{
			carvedSlice.sizeInBytes = toCarve.sizeInBytes;
			return carvedSlice;
		}

		carvedSlice.sizeInBytes = carveSizeInBytes;
		
		toCarve.content = (uint8_t*)toCarve.content + carveSizeInBytes;

		toCarve.sizeInBytes = remainingSpace;

		return carvedSlice;
	}
}
