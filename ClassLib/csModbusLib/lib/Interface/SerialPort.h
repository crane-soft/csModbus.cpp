#pragma once
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN 
#include  <Windows.h>
#include  <math.h>
#include <thread>
#include "csFifo.h"

#define MAX_RX_BUFF	256


typedef enum { NoParity, Odd, Even } Parity;
typedef enum { NoStopBits, One, Two, OnePointFive } StopBits;

class SerialPort {

public:

	SerialPort();
	void SetComParms(const char* _PortName, int _BaudRate, int _DataBits, Parity _Parity, StopBits _StopBits);
	
	void Open();
	bool IsOpen();
	void Close();
	void SetWriteTimeout(int ms);
	void SetReadTimeout(int ms);
	void DiscardOutBuffer();
	void DiscardInBuffer();
	void Write(const uint8_t * Data, int offs, int count);
	int Read(uint8_t * Data, int offs, int count);
	int ReadByte();
	int BytesToRead();

private:
	const char *PortName;
	int BaudRate;
	int DataBits;
	Parity mParity;
	StopBits mStopBits;

	// Win 32
	HANDLE comhdle;
	DWORD WriteTimeout;
	DWORD ReadTimeout;
	bool OpenWin32Port();
	void SetTheTimeouts();
};
