#pragma once
#include "MbInterface.h"
#include "MbFrame.h"
#include "ThreadUtils.h"
#include "MbSerial.h"


namespace csModbusLib {


	MbSerial::MbSerial() {}

	MbSerial::MbSerial(const char* PortName, int BaudRate)
	{
		SetComParms(PortName, BaudRate);
	}

	void MbSerial::SetComParms(const char*  PortName, int BaudRate)
	{
		SetComParms(PortName, BaudRate, 8, SerialPort::Parity::NoParity, SerialPort::StopBits::One);
	}

	void MbSerial::SetComParms(const char*  PortName, int BaudRate, int DataBits, SerialPort::Parity parity, SerialPort::StopBits stopbits)
	{
		sp.SetComParms(PortName, BaudRate, DataBits, parity, stopbits);
		oneByteTime_us = sp.GetCharTime();
	}

	int MbSerial::GetTimeOut_ms(int NumBytes)
	{
		int timeOut = (NumBytes * oneByteTime_us) / 1000;
		return timeOut + 50;    // 
	}

	bool MbSerial::Connect()
	{
		IsConnected = false;

		try {
			sp.Open();
			if (sp.IsOpen()) {
				IsConnected = true;
				sp.SetWriteTimeout(200);
			}
		} catch (int) {
			IsConnected = false;
		}
		return IsConnected;
	}

	void MbSerial::DisConnect()
	{
		if (IsConnected) {
			IsConnected = false;
			MbSleep(10);
			sp.Close();
		}
	}

	void MbSerial::WaitFrameStart(int timeout)
	{
		while (StartOfFrameDetected() == false) {
			MbSleep(10);
			if (timeout != MbInterface::InfiniteTimeout) {
				timeout -= 10;
				if (timeout <= 0)
					throw ErrorCodes::TX_TIMEOUT;


			}
			if (IsConnected == false) {
				throw ErrorCodes::CONNECTION_CLOSED;
			}
		}
	}

	void MbSerial::ReceiveHeader(int timeOut, MbRawData *RxData)
	{
		RxData->IniADUoffs();
		WaitFrameStart(timeOut);
		ReceiveBytes(RxData, 2); // Node-ID + Function-Byte
	}

	void MbSerial::ReceiveBytes(MbRawData *RxData, int count)
	{
		sp.SetReadTimeout(GetTimeOut_ms(count));
		ReceiveBytes(RxData->Data, RxData->EndIdx, count);
		RxData->EndIdx += count;
	}

	void MbSerial::ReceiveBytes(uint8_t *RxData, int offset, int count)
	{
		try {
			int bytesRead;
			while (count > 0) {
				bytesRead =  sp.Read(RxData, offset, count);
				count -= bytesRead;
				offset += bytesRead;
			}
		}
		catch (int) {
			throw ErrorCodes::TX_TIMEOUT;
		}
	}

	void MbSerial::EndOfFrame(MbRawData *RxData)
	{
		if (Check_EndOfFrame(RxData) == false) {
			// If the server receives the request, but detects a communication error (parity, LRC, CRC,  ...),
			// no response is returned. The client program will eventually process a timeout condition for the request.
			throw ErrorCodes::WRONG_CRC;
		}
	}


	void MbSerial::SendData(const uint8_t * Data, int offs, int count)
	{
		try {
			// DiscardBuffer to resync start of frame
			sp.DiscardInOut();
			sp.Write(Data, offs, count);

		} catch (int) {
			throw ErrorCodes::TX_TIMEOUT;
		}
	}

}
