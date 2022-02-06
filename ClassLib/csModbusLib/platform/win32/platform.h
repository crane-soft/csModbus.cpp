#pragma once

#include "SerialWin32.h"
#define MbSleep(ms)	std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#define DebugPrint printf
