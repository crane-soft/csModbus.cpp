#pragma once
#include "Modbus/MbFrame.h"

namespace csModbusLib
{
	class MBSFrame : public MbFrame
	{
	public:
		MBSFrame();
		int ParseMasterRequest();
		int ParseDataCount();
		void ReceiveMasterRequest(MbInterface* Interface);

#if USE_READ_WRITE_REGS
		void SaveWritaData();
		void GetRwWriteAddress();
#endif

		int ToMasterResponseMessageLength();
		bool MatchAddress(int BaseAddr, int Size);
		uint16_t GetSingleUInt16();
		coil_t GetSingleBit();
		void PutBitValues(int BaseAddr, coil_t* SrcBits);
		void PutValues(int BaseAddr, uint16_t * RegisterArray);
		bool PutValues(int BaseAddr, int size, uint16_t* RegisterArray);

		void GetBitValues(int BaseAddr, coil_t* DestBits);
		void GetValues(int BaseAddr, uint16_t *DestArray);
	private:
		bool WrMultipleData;
		bool WrSingleData;
		bool ValidAddressFound;
#if USE_READ_WRITE_REGS
		MbRawDataBuff<MbBase::MAX_FRAME_LEN> WriteData;
#endif
		int FromMasterRequestMessageLen();
		void ExceptionResponse(ExceptionCodes ErrorCode);
	};
}
