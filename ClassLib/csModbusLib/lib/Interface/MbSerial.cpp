#pragma once
#include "MbInterface.h"
#include "MbFrame.h"
#include "ThreadUtils.h"
#include "MbSerial.h"


namespace csModbusLib {


	MbSerial::MbSerial()
	{
		Init();
	}

	MbSerial::MbSerial(const char* PortName, int BaudRate, int databits, Parity parity, StopBits stopbits)
	{
		SetComParms(PortName, BaudRate, databits, parity, stopbits);
		Init();
	}

	void MbSerial::SetComParms(const char*  PortName, int BaudRate)
	{
		SetComParms(PortName, BaudRate, 8, Parity::NoParity, StopBits::One);
	}

	void MbSerial::SetComParms(const char*  PortName, int BaudRate, int DataBits, Parity parity, StopBits stopbits)
	{
		sp.SetComParms(PortName, BaudRate, DataBits, parity, stopbits);
	}

	void MbSerial::Init()
	{
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
			Sleep(50);    // wait until DataReceivedHandler has finisched
			sp.Close();
		}
	}

	bool MbSerial::ReceiveHeader(MbRawData *RxData)
	{
		sp.SetReadTimeout(1);
		if (StartOfFrameFound()) {
			sp.SetReadTimeout(rx_timeout);
			RxData->IniADUoffs();
			ReceiveBytes(RxData, 2);
			return true;
		}
		return false;
	}

	void MbSerial::ReceiveBytes(MbRawData *RxData, int count)
	{
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
			sp.DiscardOutBuffer();
			sp.DiscardInBuffer();
			sp.Write(Data, offs, count);

		} catch (int) {
			throw ErrorCodes::TX_TIMEOUT;
		}
	}

}
