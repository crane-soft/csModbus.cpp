﻿#include "MbEthernet.h"
#include "Modbus/MbFrame.h"
#include <stdint.h>


namespace csModbusLib {
	MbEthernet::MbEthernet() 
	{
		TransactionIdentifier = 0;
		mUdpClient = NULL;
	}

	void MbEthernet::SetPort(int port)
	{
		remote_port = port;
	}

	void MbEthernet::FillMBAPHeader(int Length)
	{
		++TransactionIdentifier;
		FrameData.PutUInt16(0, TransactionIdentifier);
		FrameData.PutUInt16(2, 0);
		FrameData.PutUInt16(4, (uint16_t)Length);
	}

	void MbEthernet::CheckTransactionIdentifier()
	{
		uint16_t RxIdentifier = FrameData.GetUInt16(0);
		if (RxIdentifier != TransactionIdentifier) {
			throw  ErrorCodes::WRONG_IDENTIFIER;
		}
	}


	void MbEthernet::UdpReceiveHeaderData(int timeOut)
	{
		FrameData.EndIdx = 0;
		mUdpClient->SetReceiveTimeout(timeOut);
		try {
			int readed = mUdpClient->Receive(FrameData.Data, MbBase::MAX_FRAME_LEN);
			FrameData.EndIdx = readed;

			if (FrameData.CheckEthFrameLength() > 0) {
				// we assume all framedata in one datagram
				throw  ErrorCodes::MESSAGE_INCOMPLETE;
			}

		} catch (int errCode) {
			throw errCode;
		}
	}
}
