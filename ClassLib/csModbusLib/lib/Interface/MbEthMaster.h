#pragma once
#include "MbEthernet.h"
#include "TcpClient.h"


namespace csModbusLib {

	class MbETHMaster : public MbEthernet {

	public:
		MbETHMaster(const char * host_name, int port);
		void ReceiveHeader(int timeOut);
		void FillMBAPHeader(int Length);
		void CheckTransactionIdentifier();

	protected:
		const char *remote_host;
		uint16_t TransactionIdentifier;
	};

	class MbUDPMaster : public MbETHMaster {
	public:
		MbUDPMaster(const char * host_name, int port);
		bool Connect() override;
		void DisConnect();
		void SendFrame(int Length);
		void ReceiveHeaderData(int timeOut);
		ConnectionType getConnectionType() const override { return ConnectionType::UDP_IP; };

	};

	class MbTCPMaster : public MbETHMaster {
	public:
		MbTCPMaster(const char * host_name, int port);
		bool Connect() override;
		void DisConnect();
		void SendFrame(int Length);
		ConnectionType getConnectionType() const override { return ConnectionType::TCP_IP; };

	protected:
		void ReceiveHeaderData(int timeOut);
		int ReadData(int timeOut,int offs, int length);
	private:
		TcpClient *tcpc = NULL;
	};
}
