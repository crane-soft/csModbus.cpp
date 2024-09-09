#include "MbInterface.h"
#include "MbFrame.h"
#include "MbSerial.h"

namespace csModbusLib {


	MbSerial::MbSerial(SerialPort * _sp)
	{
		sp = _sp;
	}

	SerialPort* MbSerial::getSerialPort()
	{
		return sp;
	}

	int MbSerial::GetTimeOut_ms(int serialBytesCnt)
	{
		if (serialBytesCnt == 0)
			return 0;
		int timeOut = (serialBytesCnt * oneByteTime_us) / 1000;
		return timeOut + 50;    // we need more timeout in a Windoww environement
	}

	bool MbSerial::Connect(MbRawData *Data)
	{
		MbInterface::Connect(Data);

		try {
			sp->Open();
			if (sp->IsOpen()) {
				IsConnected = true;
				sp->SetWriteTimeout(200);
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
			sp->Close();
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

	void MbSerial::ReceiveHeader(int timeOut)
	{
		MbData->Clear();
		WaitFrameStart(timeOut);
		ReceiveBytes(2); // Node-ID + Function-Byte
	}

	void MbSerial::ReceiveBytes(int count)
	{
		sp->SetReadTimeout(GetTimeOut_ms(NumOfSerialBytes(count)));
		ReceiveBytes(MbData->Data, MbData->EndIdx, count);
		MbData->EndIdx += count;
	}

	void MbSerial::ReceiveBytes(uint8_t *RxData, int offset, int count)
	{
		try {
			int bytesRead;
			while (count > 0) {
				bytesRead =  sp->Read(RxData, offset, count);
				count -= bytesRead;
				offset += bytesRead;
			}
		}
		catch (int) {
			throw ErrorCodes::TX_TIMEOUT;
		}
	}

	void MbSerial::EndOfFrame()
	{
		if (Check_EndOfFrame() == false) {
			// If the server receives the request, but detects a communication error (parity, LRC, CRC,  ...),
			// no response is returned. The client program will eventually process a timeout condition for the request.
			throw ErrorCodes::WRONG_CRC;
		}
	}

	void MbSerial::SendData(const uint8_t* Data, int count)
	{
		SendData(Data, 0, count);
	}

	void MbSerial::SendData(const uint8_t * Data, int offs, int count)
	{
		try {
			// DiscardBuffer to resync start of frame
			sp->DiscardInOut();
			sp->Write(Data, offs, count);

		} catch (int) {
			throw ErrorCodes::TX_TIMEOUT;
		}
	}

	int MbSerial::NumOfSerialBytes(int count)
	{
		return count;   // Default (RTU) , ASCI will have double of count
	}
}
