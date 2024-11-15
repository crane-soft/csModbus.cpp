﻿#pragma once
#include "Modbus/MbBase.h"
namespace csModbusLib
{
	class MbRawData;

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

		virtual bool Connect(MbRawData* Data)
		{
			this->MbData = Data;
			IsConnected = false;
			return IsConnected;
		}

		virtual void DisConnect() = 0;
		virtual void ReceiveHeader(int timeOut) = 0;
		virtual void SendFrame(int Length) = 0;

		virtual void ReceiveBytes(int count) { }
		virtual bool EndOfFrame() { return true; }
		virtual void DiscardReceived() { }
		virtual ConnectionType getConnectionType() const { return ConnectionType::NO_CONNECTION; }
	protected:
		bool IsConnected = false;
		MbRawData *MbData;
	};
}
