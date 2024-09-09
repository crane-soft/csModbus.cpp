#pragma once
#include <string>
#include "SerialWin32.h"

// Serial Communication for WIN32
// https://www.tetraedre.com/advanced/serial2.php
// https://docs.microsoft.com/en-us/previous-versions/ff802693(v=msdn.10)?redirectedfrom=MSDN



bool SerialWin32::OpenPort()
{
	HANDLE chdle;
	comhdle = 0;

	if (ComPort == 0)
		return false;

	chdle = CreateFile((char*)ComPort,
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

bool SerialWin32::IsOpen()
{
	return comhdle != 0;
}

void SerialWin32::Close()
{
	CloseHandle(comhdle);
	comhdle = 0;
}

void SerialWin32::SetTimeouts()
{
	if (comhdle > 0) {
		COMMTIMEOUTS CommTimeouts = {
			ReadTimeout,	// ReadIntervalTimeout; 
			0,				// ReadTotalTimeoutMultiplier; 
			ReadTimeout,	// ReadTotalTimeoutConstant; 
			0,				// WriteTotalTimeoutMultiplier; 
			WriteTimeout };	// WriteTotalTimeoutConstant; 

		SetCommTimeouts(comhdle, &CommTimeouts);
	}
}

void SerialWin32::DiscardInOut()
{
	if (comhdle > 0) {
		PurgeComm(comhdle, PURGE_TXABORT | PURGE_TXCLEAR);
		PurgeComm(comhdle, PURGE_RXABORT | PURGE_RXCLEAR);
	}
}

void SerialWin32::Write(const uint8_t * Data, int offs, int count)
{
	DWORD lpNumberOfBytesWritten;
	WriteFile(comhdle, &Data[offs], count, &lpNumberOfBytesWritten, NULL);
	if ((int)lpNumberOfBytesWritten < count) {
		throw - 1;
	}
}

int SerialWin32::Read(uint8_t * Data, int offs, int count)
{
	DWORD dwRead;

	if (ReadFile(comhdle, &Data[offs], count, &dwRead, NULL))
		return dwRead;
	return 0;
}


int SerialWin32::BytesToRead()
{
	DWORD comError;
	COMSTAT comStat;
	if (ClearCommError(comhdle, &comError, &comStat) == TRUE)
		return comStat.cbInQue;
	return 0;
}; 

