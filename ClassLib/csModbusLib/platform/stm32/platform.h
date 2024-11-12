#pragma once

#include <csetjmp>

#define _STM32

#define USE_SETJMP
extern 	jmp_buf global_jmp;

#define TRY int exCode; if((exCode=setjmp(global_jmp)) == 0)
#define CATCH(exval) else
#define ThrowException(ex)	longjmp(global_jmp,(int)ex)

#define RX_TIMEOUT_OFFS 10

#define MbSleep(ms)	/**/ 
#define DebugPrint(fmt,...) /**/
