#pragma once
#include "SerialPort.h"


SerialPort::SerialPort()
{
}

void SerialPort::SetComParms(const char* _PortName, int _BaudRate, int _DataBits, SerialPort::Parity _Parity, SerialPort::StopBits _StopBits)
{
	PortName = _PortName;
	BaudRate = _BaudRate;
	DataBits = _DataBits;
	mParity = _Parity;
	mStopBits = _StopBits;
}

void SerialPort::Open()
{
	if (OpenPort() == false)
		throw - 1;
	ReadTimeout = 1;
	WriteTimeout = 200;
	SetTimeouts();
}

void SerialPort::SetWriteTimeout(int ms)
{
	WriteTimeout = ms;
	SetTimeouts();
}

void SerialPort::SetReadTimeout(int ms)
{
	ReadTimeout = ms;
	SetTimeouts();
}

int SerialPort::ReadByte()
{
	uint8_t Data;
	Read(&Data, 0, 1);
	return Data;
}
