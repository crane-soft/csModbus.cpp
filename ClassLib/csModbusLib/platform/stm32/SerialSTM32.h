#pragma once
#include <stdint.h>
#include "SerialPort.h"
#define PlatformSerial	SerialSTM32

class SerialSTM32 : public SerialPort {

public:
	SerialSTM32();
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

};
