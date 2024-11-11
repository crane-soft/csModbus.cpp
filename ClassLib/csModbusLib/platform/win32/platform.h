#pragma once
#include <thread>
#include "SerialWin32.h"
#include "Timer.h"

#define TX_TIMEOUT_OFFS 20
#define MbSleep(ms)	std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#define DebugPrint printf
