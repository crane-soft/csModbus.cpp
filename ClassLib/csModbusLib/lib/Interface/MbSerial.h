#pragma once
#include "platform.h"
#include "MbInterface.h"

namespace csModbusLib {

	class MbSerial : public MbInterface {
	public:
		enum ModbusSerialType {
			RTU = 0,
			ASCII = 1
		};

	public:
		MbSerial();
		MbSerial(const char* PortName, int BaudRate);

		void SetComParms(const char*  PortName, int BaudRate);
		void SetComParms(const char*  PortName, int BaudRate, int DataBits, SerialPort::Parity _Parity, SerialPort::StopBits _StopBits);

		bool Connect();
		void DisConnect();
		void ReceiveHeader(int timeOut, MbRawData *RxData);
		void ReceiveBytes(MbRawData *RxData, int count);
		void EndOfFrame(MbRawData *RxData);

	protected:
		PlatformSerial sp;


		virtual bool StartOfFrameDetected() = 0;
		virtual bool Check_EndOfFrame(MbRawData * RxData) = 0;
		virtual int GetTimeOut_ms(int NumBytes);
		virtual void ReceiveBytes(uint8_t *RxData, int offset, int count);
		void SendData(const uint8_t * Data, int offs, int count);

	private:
		void WaitFrameStart(int timeout);
		int oneByteTime_us;
	};
}
