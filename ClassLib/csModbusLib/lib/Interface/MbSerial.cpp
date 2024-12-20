#include "Modbus/MbFrame.h"
#include "Interface/MbSerial.h"
#include "platform.h"

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
		return timeOut + TimeOutOffs;
	}

	bool MbSerial::Connect()
	{
		MbInterface::Connect();
		if (!sp)
			return false;

		sp->Open();
		if (sp->IsOpen()) {
			IsConnected = true;
			sp->SetWriteTimeout(200);
			oneByteTime_us = sp->SerialByteTime();
			TimeOutOffs = sp->TimeoutOffs();
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
		bytesRead = sp->Read(FrameData.BufferEnd(), count);
		if (bytesRead == count) {
			FrameData.Length += count;
			return;
		}
		if (bytesRead == 0)
			THROW(ErrorCodes::RX_TIMEOUT);
		THROW (ErrorCodes::CONNECTION_ERROR);
	}

	void MbSerial::ReceiveBytesEv(int count, int timeOut)
	{
		SetReadTimeout(count, timeOut);
		sp->ReadEv(FrameData.BufferEnd(), count);
		FrameData.Length += count;
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
				return;
			}
		}
	}

	void MbSerial::SendData(const uint8_t * Data, int count)
	{
		int result = sp->Write(Data, count);
		if (result == count)
			return;

#ifdef USE_EXECPTION
		if (result == 0)
			THROW(ErrorCodes::TX_TIMEOUT);
		THROW (ErrorCodes::CONNECTION_ERROR);
#endif
	}
}

