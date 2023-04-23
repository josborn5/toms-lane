namespace tl
{

	int CharStringToInt(char* toParse)
	{
		int baseCounter = 0;
		int baseLookup[8] = {
			0,
			1,
			10,
			100,
			1000,
			10000,
			100000,
			1000000
		};
		bool isNegative = (toParse[0] == '-');
		if (isNegative)
		{
			toParse++;
		}

		char* copy = toParse;
		while (*copy)
		{
			baseCounter += 1;
			copy++;
		}

		int intValue = 0;
		while (*toParse)
		{
			int digitValue = *toParse - '0';
			digitValue *= baseLookup[baseCounter];
			intValue += digitValue;

			baseCounter -= 1;
			toParse++;
		}

		return (isNegative) ? -1 * intValue : intValue;
	}

	int IntToCharString(int from, char* to)
	{
		// Special case: zero
		if (from == 0)
		{
			to[0] = '0';
			to[1] = '\0';
			return 0;
		}

		int charIndex = 0;

		// Handle negative numbers
		int safeNumber = (from < 0) ? -1 * from : from;
		if (from < 0)
		{
			to[charIndex] = '-';
			charIndex += 1;
		}

		int baseTenMultiplier = 1;
		int digit = safeNumber / baseTenMultiplier;
		int digitCount = 1;
		while (digit > 0)
		{
			baseTenMultiplier *= 10;
			digitCount += 1;
			digit = safeNumber / baseTenMultiplier;
		}

		// Could do target memory space check here since digitCount is know here???

		int workingNumber = safeNumber;
		while (baseTenMultiplier >= 10)
		{
			baseTenMultiplier /= 10;
			digit = workingNumber / baseTenMultiplier;

			to[charIndex] = '0' + (char)digit;

			workingNumber -= (digit * baseTenMultiplier);
			charIndex += 1;
		}

		to[charIndex] = '\0';
		return 0;
	}

	int IntToCharString(int from, MemorySpace to)
	{
		// Picking 10 as a basic, cover most cases char limit for an int value
		const int maxDigitCount = 10;
		if (to.sizeInBytes < (maxDigitCount * sizeof(char)))
		{
			return 1;
		}

		char* target = (char*)to.content;

		return IntToCharString(from, target);
	}

	char* CopyToChar(char* from, char* to, const char endChar)
	{
		while (*from && *from != endChar)
		{
			*to = *from;

			from++;
			to++;
		}
		*to = '\0';

		return from;
	}

	char* CopyToEndOfNumberChar(char* from, char* to)
	{
		while (*from && *from >= '0' && *from <= '9')
		{
			*to = *from;

			from++;
			to++;
		}
		*to = '\0';

		return from;
	}

	char* GetNextNumberChar(char* from)
	{
		while (*from && (*from < '0' || *from > '9'))
		{
			from++;
		}

		return from;
	}
}