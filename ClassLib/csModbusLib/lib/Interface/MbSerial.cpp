#include "Modbus/MbFrame.h"
#include "Interface/MbSerial.h"

namespace csModbusLib {

	MbSerial::MbSerial()
	{
	}

	MbSerial::MbSerial(SerialPort * _sp)
	{
		setSerialPort(_sp);
	}

	void MbSerial::setSerialPort(SerialPort* _sp)
	{
		sp = _sp;
	}

	int MbSerial::GetTimeOut_ms(int serialBytesCnt) const
	{
		if (serialBytesCnt == 0)
			return 0;
		int timeOut = (serialBytesCnt * oneByteTime_us) / 1000;
		return timeOut +  50;    // we need more timeout in a Windoww environement
	}

	bool MbSerial::Connect(MbRawData *Data)
	{
		MbInterface::Connect(Data);
		if (!sp)
			return false;
		try {
			sp->Open();
			if (sp->IsOpen()) {
				IsConnected = true;
				sp->SetWriteTimeout(200);
				oneByteTime_us = sp->SerialByteTime();
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

	void MbSerial::ReceiveBytes(int count)
	{
		ReceiveData(count, ByteCountTimeout);
	}
	
	
	void MbSerial::ReceiveData(int count, int timeout)
	{
		SetReadTimeout(count, timeout);
		int bytesRead;
		bytesRead = sp->Read(MbData->BufferEnd(), count);
		if (bytesRead == count) {
			MbData->EndIdx += count;
			return;
		}
		if (bytesRead == 0)
			throw ErrorCodes::RX_TIMEOUT;
		throw ErrorCodes::CONNECTION_ERROR;
	}

	void MbSerial::ReceiveBytesEv(int count, int timeOut)
	{
		SetReadTimeout(count, timeOut);
		sp->ReadEv(MbData->BufferEnd(), count);
		MbData->EndIdx += count;
	}

	void MbSerial::SetReadTimeout(int count, int timeout)
	{
		if (timeout == ByteCountTimeout)
			timeout = GetTimeOut_ms(count);
		sp->SetReadTimeout(timeout);

	}

	void MbSerial::DiscardReceived()
	{
		sp->DiscardInOut();
		sp->SetReadTimeout(GetTimeOut_ms(1));

		uint8_t RxData;
		int RxCount = 0;
		while (IsConnected) {
			if (sp->Read(&RxData, 1) <= 0)
				break;
			if (++RxCount == MbBase::MAX_FRAME_LEN) {
				throw ErrorCodes::CONNECTION_ERROR;
			}
			DebugPrint(".");
		}
	}

	void MbSerial::SendData(const uint8_t* Data, int count)
	{
		SendData(Data, 0, count);
	}

	void MbSerial::SendData(const uint8_t * Data, int offs, int count)
	{
		int result = sp->Write(Data, offs, count);
		if (result == count)
			return;
		if (result == 0)
			throw ErrorCodes::TX_TIMEOUT;
		throw ErrorCodes::CONNECTION_ERROR;
	}
}
