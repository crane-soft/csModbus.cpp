#pragma once
#include <stdint.h>
#include <thread>
#include "Interface/SerialPort.h"

class SerialWin32 : public SerialPort {
public:
	SerialWin32() {}
	SerialWin32(const void* _ComPort, int _BaudRate)
		: SerialPort(_ComPort, _BaudRate)	{}

	bool IsOpen() override;
	void Close() override;
	void DiscardInOut() override;
	int Write(const uint8_t * Data, int count) override;
	int Read(uint8_t * Data,  int count) override;
	void ReadEv(uint8_t* Data,  int count) override;

	int BytesToRead();	// not used anymore

protected:
	bool OpenPort() override;
	void SetTimeouts() override;

private:
	void * comhdle;

	uint8_t* RdData;
	int RdCount;
	void ReadEvFunct();
	void StopReadThread();
	bool ReadPending = false;
	bool CancelReadThread = false;
	std::thread ReadThread;
};
