#include <math.h>
#include <stdint.h>
#include "libm.h"

double floor(double x)
{
	union
	{
		double f;
		uint64_t i;
	} u = {x};

	int e = u.i >> 52 & 0x7ff;
	double_t y;

	if(e >= 0x433 || x == 0)
	{
		return x;
	}

	if(u.i >> 63)
	{
		y = (double)(x - 0x1p52) + 0x1p52 - x;
	}
	else
	{
		y = (double)(x + 0x1p52) - 0x1p52 - x;
	}

	if(e <= 0x3fe)
	{
		FORCE_EVAL(y);
		return u.i >> 63 ? -1 : 0;
	}

	if(y > 0)
	{
		return x + y - 1;
	}

	return x + y;
}