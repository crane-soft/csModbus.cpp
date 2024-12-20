#pragma once
#include "Interface/MbInterface.h"
#include "UdpClient.h"
#include <stdint.h>

namespace csModbusLib {
	class MbEthernet : public MbInterface {
	public:
		static const int MBAP_Header_Size = 6;
		enum class ModbusEthernetType {
			TCP = 0,
			UDP = 1
		};

		MbEthernet();


	protected:
		void SetPort(int port);
		virtual void ReceiveHeaderData(int timeOut) {}
		void UdpReceiveHeaderData(int timeOut);
		MbRawData* getFrameData() override 	{return &FrameData;	}

		int remote_port;
		UdpClient *mUdpClient = NULL;
		MbEthDataBuff<MbBase::MAX_FRAME_LEN> FrameData;
	};
}
