#pragma once
#include "Interface/MbEthernet.h"
#include "Modbus/MbFrame.h"

#include "TcpListener.h"
#include "Semaphore.h"

namespace csModbusLib {

	class MbETHSlave : public MbEthernet {

	public:
		MbETHSlave(int port);
		void SendFrame(int Length);

	protected:
		virtual void SendFrameData(int Length) = 0;
		virtual void FreeMessage() {}
	};

	class MbUDPSlave : public MbETHSlave {
	public:
		MbUDPSlave(int port);
		bool Connect() override;
		void DisConnect();
		void ReceiveHeader(int timeOut);
		ConnectionType getConnectionType() const override { return ConnectionType::UDP_IP; };
	protected:
		void SendFrameData(int Length);
	};

	class TcpContext;

	class MbTCPSlave : public MbETHSlave, TcpListener  {
	public:
		MbTCPSlave(int port);
		
		bool Connect()  override;
		void DisConnect();
		void ReceiveHeader(int timeOut);
		ConnectionType getConnectionType() const override { return ConnectionType::TCP_IP; };

	protected:
		void SendFrameData(int Length);
		void FreeMessage();

		// overrides TcpListener callbacks
		void OnClientAccepted(NetStream *stream);
		void OnClientRead(void * context);

	private:
		void RequestReceived(TcpContext *newContext);
		Semaphore *smRxProcess = new Semaphore();
		Semaphore *smRxDataAvail = new Semaphore();
		TcpContext *ConnectionContext = 0;
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
		MbRawDataBuff<MbBase::MAX_FRAME_LEN> FrameBuffer;
		bool closed;
		bool NewFrame = false;
		NetStream *Stream;
		int Bytes2Read;
		int ReadIndex;
	};

}
