#pragma once

#include "MbEthernet.h"
#include "TcpClient.h"


namespace csModbusLib {

	class MbETHMaster : public MbEthernet {

	public:
		MbETHMaster(const char * host_name, int port);
		void ReceiveHeader(int timeOut, MbRawData *MbData);

	protected:
		const char *remote_host;

	};

	class MbUDPMaster : public MbETHMaster {
	public:
		MbUDPMaster(const char * host_name, int port);
		bool Connect();
		void DisConnect();
		void SendFrame(MbRawData *TransmitData, int Length);
		void ReceiveHeaderData(int timeOut, MbRawData *RxData);
	};

	class MbTCPMaster : public MbETHMaster {
	public:
		MbTCPMaster(const char * host_name, int port);
		bool Connect();
		void DisConnect();
		void SendFrame(MbRawData *TransmitData, int Length);

	protected:
		void ReceiveHeaderData(int timeOut, MbRawData *RxData);
		void ReadData(int timeOut, MbRawData *RxData, int length);
	private:
		TcpClient *tcpc = NULL;
	};
}
