#include "MbUDP.h"
namespace csModbusLib
{
	MbUDPMaster::MbUDPMaster(const char * host_name, int port)
		: MbETHMaster (host_name,port)
	{
		mUdpClient = new UdpClient(host_name, port);
	}

	bool MbUDPMaster::Connect()
	{
		try {
			if (mUdpClient == 0) {
				mUdpClient = new UdpClient(remote_host, remote_port);
				mUdpClient->Connect(remote_host, remote_port);
			}
			IsConnected = true;

		} catch (int) {
			IsConnected = false;
		}
		return IsConnected;
	}

	void MbUDPMaster::DisConnect()
	{
	}

	void MbUDPMaster::SendFrame(MbRawData TransmitData, int Length)
	{

	}

	void MbUDPMaster::ReceiveBytes(MbRawData *RxData, int count)
	{
	}
	bool MbUDPMaster::RecvDataAvailable()
	{
		return false;
	}

}


    