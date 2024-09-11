#pragma once
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <thread>
#include "Interface/SerialPort.h"

class SerialWin32 : public SerialPort {
public:
	SerialWin32() {}
	SerialWin32(const void* _ComPort, int _BaudRate)
		: SerialPort(_ComPort, _BaudRate)	{}

	bool IsOpen();
	void Close();
	void DiscardInOut();
	void Write(const uint8_t * Data, int offs, int count);
	int Read(uint8_t * Data, int offs, int count);
	int BytesToRead();

protected:
	bool OpenPort();
	void SetTimeouts();
	bool StartEventHandler() override;	
	bool StopEventHandler();
private:
	void HandleEvents();
	std::thread* EventThread = NULL;
	HANDLE comhdle;
};
