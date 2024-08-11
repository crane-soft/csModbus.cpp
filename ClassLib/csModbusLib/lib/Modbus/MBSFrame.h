#pragma once
#include "MbFrame.h"

namespace csModbusLib
{
	class MBSFrame : public MbFrame
	{
	public:
		MBSFrame();
		int ParseMasterRequest();
		int ParseDataCount();
		void SaveWritaData();
		void ReceiveMasterRequest(MbInterface *Interface);
		void GetRwWriteAddress();
		int ToMasterResponseMessageLength();
		bool MatchAddress(int BaseAddr, int Size);
		uint16_t GetRequestSingleUInt16();
		coil_t GetRequestSingleBit();
		void PutResponseBitValues(int BaseAddr, coil_t* SrcBits);
		void PutResponseValues(int BaseAddr, uint16_t * RegisterArray);
		void GetRequestBitValues(int BaseAddr, coil_t* DestBits);
		void GetRequestValues(int BaseAddr, uint16_t *DestArray);

	private:
		bool WrMultipleData;
		bool WrSingleData;
		bool ValidAddressFound;
		MbRawData *WriteData;
		int FromMasterRequestMessageLen();
		void ExceptionResponse(ExceptionCodes ErrorCode);
	};
}
