#pragma once

#include <csetjmp>

#define _STM32
#define USE_EXECPTION

#define RX_TIMEOUT_OFFS 10
#define MbSleep(ms)	/**/
#define DebugPrint(fmt,...) /**/



#ifdef USE_EXECPTION
#define USE_SETJMP
extern 	jmp_buf global_jmp;
extern int try_level;
extern void ThrowException(int ex);

#define TRY 		 int exCode; if((exCode=setjmp(global_jmp)) == 0)
#define CATCH(exval) else
#define THROW(ex)	 ThrowException((int)ex)

#else	// exceptions (setjmp) not allowed
extern int NoExceptions();
#define TRY 		int exCode=0; if(NoExceptions() == 0)
#define THROW(ex)	NoExceptions()
#define CATCH(exval) else

#endif
