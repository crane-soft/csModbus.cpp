#pragma once
#include "Modbus/MbBase.h"
#include "Modbus/MbRawData.h"

namespace csModbusLib
{
	class MbInterface
	{
	public:
		enum class ConnectionType
		{
			NO_CONNECTION = 0,
			SERIAL_RTU = 1,
			SERIAL_ASCII = 2,
			TCP_IP = 3,
			UDP_IP = 4
		};

		static const int ByteCountTimeout = -1;
		static const int InfiniteTimeout = 0;
		static const int ResponseTimeout = 200;

		MbInterface()	{}

		virtual bool Connect()
		{
			IsConnected = false;
			return IsConnected;
		}

		virtual void DisConnect() = 0;
		virtual void ReceiveHeader(int timeOut) = 0;
		virtual void ReceiveBytes(int count) { }
		virtual bool EndOfFrame() { return true; }
		virtual void SendFrame(int Length) = 0;
		virtual void DiscardReceived() { }
		virtual ConnectionType getConnectionType() const { return ConnectionType::NO_CONNECTION; }
		virtual MbRawData* getFrameData() = 0;
	protected:
		bool IsConnected = false;
	};
}
