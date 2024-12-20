#include "MbEthernet.h"
#include "Modbus/MbFrame.h"
#include <stdint.h>


namespace csModbusLib {
	MbEthernet::MbEthernet() 
	{
		mUdpClient = NULL;
	}

	void MbEthernet::SetPort(int port)
	{
		remote_port = port;
	}

	void MbEthernet::UdpReceiveHeaderData(int timeOut)
	{
		mUdpClient->SetReceiveTimeout(timeOut);
		try {
			int readed = mUdpClient->Receive(FrameData.EthData, FrameData.EthSize);

			if (FrameData.CheckEthFrameLength(readed) > 0) {
				// we assume all framedata in one datagram
				throw  ErrorCodes::MESSAGE_INCOMPLETE;
			}
			FrameData.Length = readed-6;
		} catch (int errCode) {
			throw errCode;
		}
	}
}
