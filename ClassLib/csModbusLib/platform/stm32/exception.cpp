#include "platform.h"

#ifndef USE_EXECPTION
int NoExceptions() {	while(1);}
#endif

#ifdef USE_SETJMP
int try_level = 0;
jmp_buf global_jmp;

void ThrowException(int ex)
{
	if (try_level > 0)
		longjmp(global_jmp,ex);
}

#endif
