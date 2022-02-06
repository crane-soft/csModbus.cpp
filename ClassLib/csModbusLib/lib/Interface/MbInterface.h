#pragma once

namespace csModbusLib
{
	class MbRawData;

	class MbInterface
	{
	public:
		static const int InfiniteTimeout = -1;
		static const int ResponseTimeout = 200;

		MbInterface()	{}

		virtual bool Connect() = 0;
		virtual void DisConnect() = 0;
		virtual void ReceiveHeader(int timeOut, MbRawData *MbData) = 0;
		virtual void SendFrame(MbRawData *TransmitData, int Length) = 0;

		virtual void ReceiveBytes(MbRawData *RxData, int count) { }
		virtual void EndOfFrame(MbRawData *RxData) { }

		bool ReConnect()
		{
			DisConnect();
			return Connect();
		}

	protected:
		bool IsConnected = false;

	};
}
