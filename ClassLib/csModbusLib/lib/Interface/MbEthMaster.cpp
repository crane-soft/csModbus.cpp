#include "MbEthMaster.h"
#include "Modbus/MbFrame.h"
namespace csModbusLib {

	// -------------------------------------------------------------------
	// Ethernet Master
	// -------------------------------------------------------------------
	MbETHMaster::MbETHMaster(const char * host_name, int port)
	{
		remote_host = host_name;
		TransactionIdentifier = 0;
		SetPort(port);
	}

	void MbETHMaster::ReceiveHeader(int timeOut)
	{
		ReceiveHeaderData(timeOut);
		CheckTransactionIdentifier();
	}

	void MbETHMaster::FillMBAPHeader(int Length)
	{
		++TransactionIdentifier;
		FrameData.FillMBAPHeader(Length, TransactionIdentifier);
	}

	void MbETHMaster::CheckTransactionIdentifier()
	{
		uint16_t RxIdentifier = FrameData.GetUInt16(-6);
		if (RxIdentifier != TransactionIdentifier) {
			throw  ErrorCodes::WRONG_IDENTIFIER;
		}
	}

	// -------------------------------------------------------------------
	// Ethernet UDP-Master
	// -------------------------------------------------------------------
	MbUDPMaster::MbUDPMaster(const char * host_name, int port)
		:MbETHMaster(host_name, port)
	{
	}

	bool MbUDPMaster::Connect()
	{
		MbInterface::Connect();
		try {
			if (mUdpClient == 0) {
				mUdpClient = new UdpClient(remote_host, remote_port);
			}
			IsConnected = true;

		} catch (int) {
			IsConnected = false;
		}
		return IsConnected;
	}

	void MbUDPMaster::DisConnect()
	{
		if (mUdpClient != 0)
			mUdpClient->Close();
		mUdpClient = 0;
		IsConnected = false;
	}

	void MbUDPMaster::SendFrame(int Length)
	{
		FillMBAPHeader(Length);
		mUdpClient->Send(FrameData.EthData, Length + MBAP_Header_Size);
	}

	void MbUDPMaster::ReceiveHeaderData(int timeOut)
	{
		UdpReceiveHeaderData(timeOut);
	}

	// -------------------------------------------------------------------
	// Ethernet TCP-Master
	// -------------------------------------------------------------------
	MbTCPMaster::MbTCPMaster(const char * host_name, int port)
		: MbETHMaster(host_name, port)
	{
		tcpc = new TcpClient();
	}

	bool MbTCPMaster::Connect()
	{
		MbInterface::Connect();
		if (tcpc == NULL)
			tcpc = new TcpClient();
		try {
			tcpc->Connect(remote_host, remote_port);
			//if (tcpc.Connected) {
			//	nwStream = tcpc.GetStream();
			//	IsConnected = true;
			//}
		} catch (int) {
			IsConnected = false;
		}
		return IsConnected;

	}

	void MbTCPMaster::DisConnect()
	{
		if (tcpc != NULL)
			tcpc->Close();
		tcpc = NULL;
		IsConnected = false;

	}

	void MbTCPMaster::SendFrame(int Length)
	{
		FillMBAPHeader(Length);
		tcpc->Send (FrameData.EthData, Length + MBAP_Header_Size);
	}

	void MbTCPMaster::ReceiveHeaderData(int timeOut)
	{
		int readed = ReadData(ResponseTimeout,0, 8);
		int bytes2read = FrameData.CheckEthFrameLength(readed);
		if (bytes2read > 0) {
			ReadData(50, 8, bytes2read);
		}
	}

	int MbTCPMaster::ReadData(int timeOut, int offs, int length)
	{
		try {
			tcpc->SetReceiveTimeout(timeOut);
			return tcpc->Receive(&FrameData.EthData[offs], length);
		} catch (int) {
			throw ErrorCodes::RX_TIMEOUT;
		}
	}
}
