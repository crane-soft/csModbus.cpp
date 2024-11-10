#include "MbEthMaster.h"
#include "Modbus/MbFrame.h"
namespace csModbusLib {

	// -------------------------------------------------------------------
	// Ethernet Master
	// -------------------------------------------------------------------
	MbETHMaster::MbETHMaster(const char * host_name, int port)
	{
		remote_host = host_name;
		SetPort(port);
	}

	void MbETHMaster::ReceiveHeader(int timeOut)
	{
		MbData->EndIdx = 0;
		ReceiveHeaderData(timeOut);
		CheckTransactionIdentifier();
	}

	// -------------------------------------------------------------------
	// Ethernet UDP-Master
	// -------------------------------------------------------------------
	MbUDPMaster::MbUDPMaster(const char * host_name, int port)
		:MbETHMaster(host_name, port)
	{
	}

	bool MbUDPMaster::Connect(MbRawData *Data)
	{
		MbInterface::Connect(Data);
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
		mUdpClient->Send(MbData->Data, Length + MBAP_Header_Size);
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

	bool MbTCPMaster::Connect(MbRawData* Data)
	{
		MbInterface::Connect(Data);
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
		tcpc->Send (MbData->Data, Length + MBAP_Header_Size);
	}

	void MbTCPMaster::ReceiveHeaderData(int timeOut)
	{
		ReadData(ResponseTimeout, 8);
		int bytes2read = MbData->CheckEthFrameLength();
		if (bytes2read > 0) {
			ReadData(50, bytes2read);
		}
	}

	void MbTCPMaster::ReadData(int timeOut, int length)
	{
		try {
			tcpc->SetReceiveTimeout(timeOut);
			int readed = tcpc->Receive(MbData->BufferEnd(), length);
			MbData->EndIdx += readed;
		} catch (int) {
			throw ErrorCodes::RX_TIMEOUT;
		}
	}
}
