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

	void CopyToChar(char* from, char* to, const char endChar)
	{
		while (*from && *from != endChar)
		{
			*to = *from;

			from++;
			to++;
		}
		*to = '\0';
	}
}