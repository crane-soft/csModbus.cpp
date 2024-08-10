#pragma once

#include "MbEthernet.h"
#include "TcpClient.h"


namespace csModbusLib {

	class MbETHMaster : public MbEthernet {

	public:
		MbETHMaster(const char * host_name, int port);
		void ReceiveHeader(int timeOut);

	protected:
		const char *remote_host;

	};

	class MbUDPMaster : public MbETHMaster {
	public:
		MbUDPMaster(const char * host_name, int port);
		bool Connect(MbRawData *Data);
		void DisConnect();
		void SendFrame(int Length);
		void ReceiveHeaderData(int timeOut);
	};

	class MbTCPMaster : public MbETHMaster {
	public:
		MbTCPMaster(const char * host_name, int port);
		bool Connect(MbRawData* Data);
		void DisConnect();
		void SendFrame(int Length);

	protected:
		void ReceiveHeaderData(int timeOut);
		void ReadData(int timeOut, int length);
	private:
		TcpClient *tcpc = NULL;
	};
}
