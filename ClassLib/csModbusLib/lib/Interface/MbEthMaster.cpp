#include "MbEthMaster.h"

namespace csModbusLib {

	// -------------------------------------------------------------------
	// Ethernet Master
	// -------------------------------------------------------------------
	MbETHMaster::MbETHMaster(const char * host_name, int port)
	{
		remote_host = host_name;
		SetPort(port);
	}

	void MbETHMaster::ReceiveHeader(int timeOut, MbRawData *MbData)
	{
		MbData->EndIdx = 0;
		ReceiveHeaderData(timeOut, MbData);
		CheckTransactionIdentifier(MbData);
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

	void MbUDPMaster::SendFrame(MbRawData *TransmitData, int Length)
	{
		FillMBAPHeader(TransmitData, Length);
		mUdpClient->Send(TransmitData->Data, Length + MBAP_Header_Size);
	}

	void MbUDPMaster::ReceiveHeaderData(int timeOut, MbRawData *RxData)
	{
		UdpReceiveHeaderData(timeOut, RxData);
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

	void MbTCPMaster::SendFrame(MbRawData *TransmitData, int Length)
	{
		FillMBAPHeader(TransmitData, Length);
		tcpc->Send (TransmitData->Data, Length + MBAP_Header_Size);
	}

	void MbTCPMaster::ReceiveHeaderData(int timeOut, MbRawData *RxData)
	{
		ReadData(ResponseTimeout, RxData, 8);
		int bytes2read = RxData->CheckEthFrameLength();
		if (bytes2read > 0) {
			ReadData(50, RxData, bytes2read);
		}
	}

	void MbTCPMaster::ReadData(int timeOut, MbRawData *RxData, int length)
	{
		try {
			tcpc->SetReceiveTimeout(timeOut);
			int readed = tcpc->Receive(RxData->GetBuffTail(), length);
			RxData->EndIdx += readed;
		} catch (int) {
			throw ErrorCodes::RX_TIMEOUT;
		}
	}
}
