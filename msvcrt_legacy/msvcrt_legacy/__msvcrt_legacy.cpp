#include "stdafx.h"

extern "C" FILE* __cdecl __iob_func(void)
{
	static FILE iob[] = { *stdin, *stdout, *stderr };
	return iob;
}