#pragma once
#include <thread>

#define TRY try
#define CATCH(exval) catch(int exval)
#define ThrowException(ex)	throw ex

#define RX_TIMEOUT_OFFS 20
#define MbSleep(ms)	std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#define DebugPrint printf
