#include <stdlib.h>

int abs(int i)
{
	return i < 0 ? -i : i;
}

long labs(long i)
{
	return i < 0 ? -i : i;
}

long long llabs(long long i)
{
	return i < 0 ? -i : i;
}