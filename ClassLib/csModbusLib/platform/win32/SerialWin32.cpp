#pragma once
#include <string>
#include "SerialWin32.h"
#include "platform.h"
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
	if (ReadPending) {
		StopReadThread();
	}
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

int SerialWin32::Write(const uint8_t * Data, int offs, int count)
{
	DWORD lpNumberOfBytesWritten;
	if (WriteFile(comhdle, &Data[offs], count, &lpNumberOfBytesWritten, NULL)) {
		if ((int)lpNumberOfBytesWritten < count) {
			return 0; // Timeout
		}
		return count;
	}
	return -1;	// some error use GetLastError here
}

int SerialWin32::Read(uint8_t * Data, int count)
{
	DWORD dwRead;
	if (ReadFile(comhdle, Data, count, &dwRead, NULL)) {
		if ((int)dwRead < count)
			return 0; // Timeout
		return dwRead;
	}
	return - 1;	// some error use GetLastError here
}

int SerialWin32::BytesToRead()
{
	DWORD comError;
	COMSTAT comStat;
	if (ClearCommError(comhdle, &comError, &comStat) == TRUE)
		return comStat.cbInQue;
	return 0;
}; 

void SerialWin32::ReadEv(uint8_t* Data,  int count)
{
	RdData = Data;
	RdCount = count;

	if (ReadPending == false) {
		if (ReadThread.joinable())
			ReadThread.join();
		ReadPending = true;
		CancelReadThread = false;
		ReadThread = std::thread{ [this]() { ReadEvFunct(); } };
	}
}

void SerialWin32::StopReadThread()
{
	CancelReadThread = true;
	CancelSynchronousIo(ReadThread.native_handle());

	if (ReadThread.joinable())
		ReadThread.join();
}

void SerialWin32::ReadEvFunct()
{
	int cbResult = 0;
	do  {
		int RdResult = Read(RdData, RdCount);
		if (CancelReadThread)
			break;
		if (ReadCallBack) {
			cbResult = ReadCallBack(RdResult);
		}
	} while (cbResult != 0);
	ReadPending = false;
}

