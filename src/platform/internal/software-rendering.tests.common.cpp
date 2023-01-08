const uint32_t EMPTY = 0x000000;
const uint32_t FILLED = 0xFFFFFF;

void ClearPixelArray(uint32_t* pixelArray, int arrayLength)
{
	for (int i = 0; i < arrayLength; i += 1)
	{
		pixelArray[i] = EMPTY;
	}
}