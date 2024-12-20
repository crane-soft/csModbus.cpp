#include "Interface/SerialPort.h"

SerialPort::SerialPort()
{
	ComPort = 0;
}

SerialPort::SerialPort(const void* _ComPort, int _BaudRate)
{
	SetComParms(_ComPort, _BaudRate);
}
void SerialPort::SetComParms(const void* _ComPort, int _BaudRate)
{
	SetComParms(_ComPort, _BaudRate, 8, Parity::NoParity, StopBits::One);
}

void SerialPort::SetComParms(const void* _ComPort, int _BaudRate, int _DataBits, SerialPort::Parity _Parity, SerialPort::StopBits _StopBits)
{
	ComPort = _ComPort;
	BaudRate = _BaudRate;
	DataBits = _DataBits;
	mParity = _Parity;
	mStopBits = _StopBits;
}

void SerialPort::setCallback(ReadCallback_t callBack)
{
	ReadCallBack = callBack;
	InitCallBack();
}

bool SerialPort::Open()
{
	if (OpenPort() == false)
		return false;
	ReadTimeout = 1;
	WriteTimeout = 200;
	SetTimeouts();
	return true;
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
	if (Read(&Data, 1) == 1)
		return Data;
	return 0;
}

int SerialPort::SerialByteTime() const
{
	int nbits = 1 + DataBits;
	nbits += mParity == Parity::NoParity ? 0 : 1;
	switch (mStopBits) {
	case StopBits::One:
		nbits += 1;
		break;

	case StopBits::OnePointFive: // Ceiling
	case StopBits::Two:
		nbits += 2;
		break;
	default:
		break;
	}
	return (1000000L * nbits) / BaudRate;
}
