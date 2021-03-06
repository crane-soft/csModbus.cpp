#pragma once
#include "MbEthernet.h"
#include "MbFrame.h"
#include "UdpClient.h"
#include "TcpClient.h"
#include "TcpListener.h"
#include "Semaphore.h"

namespace csModbusLib {

	class MbETHSlave : public MbEthernet {

	public:
		MbETHSlave(int port);
		virtual void SendFrame(MbRawData *TransmitData, int Length);

	protected:
		virtual void SendFrameData(uint8_t *data, int Length) = 0;
		virtual void FreeMessage() {}
	};

	class MbUDPSlave : public MbETHSlave {
	public:
		MbUDPSlave(int port);
		bool Connect();
		void DisConnect();
		void ReceiveHeader(int timeOut, MbRawData *RxData);
	protected:
		void SendFrameData(uint8_t *data, int Length);
	};

	class TcpContext;

	class MbTCPSlave : public MbETHSlave, TcpListener  {
	public:
		MbTCPSlave(int port);
		
		bool Connect();
		void DisConnect();
		void ReceiveHeader(int timeOut, MbRawData *RxData);
	protected:
		void SendFrameData(uint8_t *data, int Length);
		void FreeMessage();

		// overrides TcpListener callbacks
		void OnClientAccepted(NetStream *stream);
		void OnClientRead(void * context);

	private:
		void RequestReceived(TcpContext *newContext);
		Semaphore *smRxProcess = new Semaphore();
		Semaphore *smRxDataAvail = new Semaphore();
		TcpContext *ConnectionContext;
	};

	class TcpContext {
	public:
		TcpContext(NetStream *aStream);
		void SendFrame(uint8_t *data, int Length);
		void BeginNewFrame();
		void BeginReadFrameData();
		bool EndReceive();
		void CopyRxData(MbRawData *RxData);

	private:
		MbRawData FrameBuffer;
		bool closed;
		bool NewFrame = false;
		NetStream *Stream;
		int Bytes2Read;
		int ReadIndex;
	};

}
