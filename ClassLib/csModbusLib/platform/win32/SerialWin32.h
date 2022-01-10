#pragma once
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN 
#include  <Windows.h>

#include "SerialPort.h"
#define PlatformSerial	SerialWin32

class SerialWin32 : public SerialPort {

public:
	SerialWin32();
	bool IsOpen();
	void Close();
	void DiscardInOut();
	void Write(const uint8_t * Data, int offs, int count);
	int Read(uint8_t * Data, int offs, int count);
	int BytesToRead();

protected:
	bool OpenPort();
	void SetTimeouts();
private:
	HANDLE comhdle;
};
