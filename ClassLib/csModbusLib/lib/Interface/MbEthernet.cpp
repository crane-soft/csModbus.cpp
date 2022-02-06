#include "MbEthernet.h"
#include "MbFrame.h"
#include "UdpClient.h"
#include <stdint.h>


namespace csModbusLib {
	MbEthernet::MbEthernet() {
		TransactionIdentifier = 0;
		mUdpClient = NULL;
	}

	void MbEthernet::SetPort(int port)
	{
		remote_port = port;
	}

	void MbEthernet::FillMBAPHeader(MbRawData *TransmitData, int Length)
	{
		++TransactionIdentifier;
		TransmitData->PutUInt16(0, TransactionIdentifier);
		TransmitData->PutUInt16(2, 0);
		TransmitData->PutUInt16(4, (uint16_t)Length);
	}

	void MbEthernet::CheckTransactionIdentifier(MbRawData *ReceivMessage)
	{
		uint16_t RxIdentifier = ReceivMessage->GetUInt16(0);
		if (RxIdentifier != TransactionIdentifier) {
			throw  ErrorCodes::WRONG_IDENTIFIER;
		}
	}


	void MbEthernet::UdpReceiveHeaderData(int timeOut, MbRawData *RxData)
	{
		RxData->EndIdx = 0;
		mUdpClient->SetReceiveTimeout(timeOut);
		try {
			int readed = mUdpClient->Receive(RxData->Data, MbBase::MAX_FRAME_LEN);
			RxData->EndIdx = readed;

			if (RxData->CheckEthFrameLength() > 0) {
				// we assume all framedata in one datagram
				throw  ErrorCodes::MESSAGE_INCOMPLETE;
			}

		} catch (int errCode) {
			throw errCode;
		}
	}
}
