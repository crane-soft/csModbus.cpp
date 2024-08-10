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
		virtual void EndOfFrame() { }

	protected:
		bool IsConnected = false;
		MbRawData *MbData;

	};
}
