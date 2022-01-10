#pragma once
#include "Stopwatch.h"

namespace csModbusLib
{
	class MbRawData;

	class MbInterface
	{
	const int DEFAULT_RX_TIMEOUT = 600;
	protected: int rx_timeout;
			   Stopwatch timeoutTmer;
			   bool IsConnected;

	public:
		MbInterface()
		{
			rx_timeout = DEFAULT_RX_TIMEOUT;
		}

		virtual bool Connect() = 0;
		virtual void DisConnect() = 0;
		virtual bool ReceiveHeader(MbRawData *MbData) = 0;
		virtual void SendFrame(MbRawData *TransmitData, int Length) = 0;

		virtual void ReceiveBytes(MbRawData *RxData, int count) { }
		virtual void EndOfFrame(MbRawData *RxData) { }

		bool ReConnect()
		{
			DisConnect();
			return Connect();
			// TODO im Aufruf error abfangen
		}

	};
}
