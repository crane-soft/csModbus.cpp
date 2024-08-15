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
		uint16_t GetSingleUInt16();
		coil_t GetSingleBit();
		void PutBitValues(int BaseAddr, coil_t* SrcBits);
		void PutValues(int BaseAddr, uint16_t * RegisterArray);
		void GetBitValues(int BaseAddr, coil_t* DestBits);
		void GetValues(int BaseAddr, uint16_t *DestArray);

	private:
		bool WrMultipleData;
		bool WrSingleData;
		bool ValidAddressFound;
		MbRawDataBuff<MbBase::MAX_FRAME_LEN> WriteData;
		int FromMasterRequestMessageLen();
		void ExceptionResponse(ExceptionCodes ErrorCode);
	};
}
