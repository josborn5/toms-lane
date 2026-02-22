const char* CopyToEndOfNumberChar(const char* from, char* to)
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

const char* GetNextNumberChar(const char* from)
{
	while (*from && (*from < '0' || *from > '9'))
	{
		from++;
	}

	return from;
}

