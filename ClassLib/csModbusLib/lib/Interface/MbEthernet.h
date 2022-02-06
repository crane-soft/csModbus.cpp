#pragma once
#include "MbInterface.h"
#include "MbFrame.h"
#include "UdpClient.h"
#include <stdint.h>

namespace csModbusLib {
	class MbEthernet : public MbInterface {
	public:
		static const int MBAP_Header_Size = 6;
		enum ModbusEthernetType {
			TCP = 0,
			UDP = 1
		};

		MbEthernet();


	protected:
		void SetPort(int port);
		void FillMBAPHeader(MbRawData *TransmitData, int Length);
		void CheckTransactionIdentifier(MbRawData *ReceivMessage);
		virtual void ReceiveHeaderData(int timeOut, MbRawData *MbData) {}
		void UdpReceiveHeaderData(int timeOut, MbRawData *RxData);

		int remote_port;
		UdpClient *mUdpClient = NULL;

	private:
		uint16_t TransactionIdentifier;
	};
}
