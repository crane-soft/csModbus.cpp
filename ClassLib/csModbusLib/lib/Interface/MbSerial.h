#pragma once
#include "platform.h"
#include "MbInterface.h"

namespace csModbusLib {

	class MbSerial : public MbInterface {
	public:
		enum class ModbusSerialType {
			RTU = 0,
			ASCII = 1
		};

	public:
		MbSerial();
		MbSerial(const char* PortName, int BaudRate);

		void SetComParms(const char*  PortName, int BaudRate);
		void SetComParms(const char*  PortName, int BaudRate, int DataBits, SerialPort::Parity _Parity, SerialPort::StopBits _StopBits);
		PlatformSerial * getSerialPort();

		bool Connect(MbRawData* Data);
		void DisConnect();
		void ReceiveHeader(int timeOut);
		void ReceiveBytes(int count);
		void EndOfFrame();
		virtual int NumOfSerialBytes(int count);
		virtual bool StartOfFrameDetected() = 0;
		virtual int EndOffFrameLenthth() = 0;
		virtual int GetTimeOut_ms(int NumBytes);

	protected:
		PlatformSerial sp;

		virtual bool Check_EndOfFrame() = 0;
		virtual void ReceiveBytes(uint8_t *RxData, int offset, int count);
		void SendData(const uint8_t * Data, int offs, int count);

	private:
		void WaitFrameStart(int timeout);
		int oneByteTime_us;
	};
}
