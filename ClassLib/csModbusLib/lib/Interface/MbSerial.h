#pragma once
#include "platform.h"
#include "Interface/MbInterface.h"

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

		bool Connect(MbRawData* Data) override;
		void DisConnect() override;

		void ReceiveBytes(int count) override;
		void ReceiveBytesEv(int count, int timeOut = ByteCountTimeout);

		void DiscardReceived() override;
		virtual void Check_EndOfFrame() = 0;

	protected:
		SerialPort *sp;

		virtual void WaitFrameStart(int timeout) {}
		virtual void ReceiveData(int count, int timeout = ByteCountTimeout);
		void SetReadTimeout(int count, int timeout);
		void SendData(const uint8_t* Data, int count);
		void SendData(const uint8_t * Data, int offs, int count);
	private:
		int GetTimeOut_ms(int NumBytes) const;
		int oneByteTime_us;
	};
}
