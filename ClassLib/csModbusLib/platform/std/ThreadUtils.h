#pragma once
#include <thread>
#include <chrono>

/*
namespace ModbusLib {
	namespace Threading {
		class Thread {
		public:
			typedef void(*ThreadStart)();
			Thread(ThreadStart start)
			{
				new std::thread(start, this);
			}
		public:
			static void Sleep(int ms) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		};
	}
}
*/
