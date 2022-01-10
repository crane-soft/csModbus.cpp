#pragma once
#include <string>
#include "SerialPort.h"

// Serial Communication for WIN32
// https://www.tetraedre.com/advanced/serial2.php
// https://docs.microsoft.com/en-us/previous-versions/ff802693(v=msdn.10)?redirectedfrom=MSDN


SerialPort::SerialPort()
{
	comhdle = 0;
}

void SerialPort::SetComParms(const char* _PortName, int _BaudRate, int _DataBits, Parity _Parity, StopBits _StopBits)
{
	PortName = _PortName;
	BaudRate = _BaudRate;
	DataBits = _DataBits;
	mParity = _Parity;
	mStopBits = _StopBits;
}

void SerialPort::Open()
{
	if (OpenWin32Port() == false)
		throw - 1;
	ReadTimeout = 1;
	WriteTimeout = 200;
	SetTheTimeouts();
}

bool SerialPort::OpenWin32Port()
{
	HANDLE chdle;
	comhdle = 0;

	chdle = CreateFile(PortName,
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		0);

	if (chdle == INVALID_HANDLE_VALUE)
		return false;

	DCB dcb;
	FillMemory(&dcb, sizeof(dcb), 0);

	if (!GetCommState(chdle, &dcb)) {
		return false;
	}

	dcb.BaudRate = BaudRate;
	dcb.ByteSize = DataBits;
	switch (mParity) {
	case Parity::Even:
		dcb.Parity = EVENPARITY;
		break;
	case Parity::Odd:
		dcb.Parity = ODDPARITY;
		break;
	default:
		dcb.Parity = NOPARITY;
	}
	switch (mStopBits) {
	case StopBits::OnePointFive:
		dcb.StopBits = ONE5STOPBITS;
		break;
	case StopBits::Two:
		dcb.StopBits = TWOSTOPBITS;
		break;
	default:
		dcb.StopBits = ONESTOPBIT;
	}

	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fAbortOnError = FALSE;

	if (!SetCommState(chdle, &dcb))
		return false;

	comhdle = chdle;
	return true;
}

bool SerialPort::IsOpen()
{
	return comhdle != 0;
}

void SerialPort::Close()
{
	CloseHandle(comhdle);
	comhdle = 0;
}

void SerialPort::SetWriteTimeout(int ms)
{
	WriteTimeout = ms;
	SetTheTimeouts();
}

void SerialPort::SetReadTimeout(int ms)
{
	ReadTimeout = ms;
	SetTheTimeouts();
}

void SerialPort::SetTheTimeouts()
{
	if (comhdle > 0) {
		COMMTIMEOUTS CommTimeouts = {
			0,		// ReadIntervalTimeout; 
			0,		// ReadTotalTimeoutMultiplier; 
			0,		// ReadTotalTimeoutConstant; 
			0,		// WriteTotalTimeoutMultiplier; 
			200 };	// WriteTotalTimeoutConstant; 

		SetCommTimeouts(comhdle, &CommTimeouts);
	}
}

void SerialPort::DiscardOutBuffer()
{
	if (comhdle > 0) {
		PurgeComm(comhdle, PURGE_TXABORT | PURGE_TXCLEAR);
	}
}

void SerialPort::DiscardInBuffer()
{
	if (comhdle > 0) {
		PurgeComm(comhdle, PURGE_RXABORT | PURGE_RXCLEAR);
	}

}

void SerialPort::Write(const uint8_t * Data, int offs, int count)
{
	DWORD lpNumberOfBytesWritten;
	WriteFile(comhdle, &Data[offs], count, &lpNumberOfBytesWritten, NULL);
	if ((int)lpNumberOfBytesWritten < count) {
		throw - 1;
	}
}

int SerialPort::Read(uint8_t * Data, int offs, int count)
{
	DWORD dwRead;

	if (ReadFile(comhdle, &Data[offs], count, &dwRead, NULL))
		return dwRead;
	return 0;
}

int SerialPort::ReadByte()
{
	uint8_t Data;
	Read(&Data, 0, 1);
	return Data;
}

int SerialPort::BytesToRead()
{
	DWORD comError;
	COMSTAT comStat;
	if (ClearCommError(comhdle, &comError, &comStat) == TRUE)
		return comStat.cbInQue;
	return 0;
};
