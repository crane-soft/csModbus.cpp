#pragma once
#include "platform.h"
#include "Interface/MbInterface.h"
#include "SerialPort.h"
namespace csModbusLib {

	class MbSerial : public MbInterface {
	public:
		enum class ModbusSerialType {
			RTU = 0,
			ASCII = 1
		};

	public:
		MbSerial();
		MbSerial(SerialPort * _sp);
		void setSerialPort(SerialPort* _sp);
		void setCallback(SerialPort::ReadCallback_t callBack) {	sp->setCallback(callBack);	}
		MbRawData* getFrameData() override
			{ return &FrameData; }

		bool Connect() override;
		void DisConnect() override;

		void ReceiveBytes(int count) override;
		void ReceiveBytesEv(int count, int timeOut = ByteCountTimeout);

		void DiscardReceived() override;
		virtual bool Check_EndOfFrame() = 0;
		int TimeOutOffs;
	protected:
		virtual void ReceiveData(int count, int timeout = ByteCountTimeout);
		void SetReadTimeout(int count, int timeout);
		void SendData(const uint8_t* Data, int count);
		SerialPort* sp;
		MbRawDataBuff<MbBase::MAX_FRAME_LEN> FrameData;

	private:
		int GetTimeOut_ms(int NumBytes) const;
		int oneByteTime_us;
	};
}
