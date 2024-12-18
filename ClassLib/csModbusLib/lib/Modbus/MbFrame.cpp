#include "Modbus/MbFrame.h"
#include <cstring>

namespace csModbusLib
{
	MbFrame::MbFrame()
	{
		ExceptionCode = ExceptionCodes::NO_EXCEPTION;
	}
	
	void MbFrame::GetBitData(coil_t * DestBits, int DestIdx, int FrameIdx)
	{
		DestBits += DestIdx;
		uint8_t * SrcByte = &RawData->Data[FrameIdx];

		int bitCnt = 0;
		uint8_t dataByte = 0;

		for (int i = 0; i < DataCount; ++i) {
			if (bitCnt == 0) {
				dataByte = *SrcByte++;
			}
			int b = dataByte & 1;
			*DestBits++ = (coil_t)b;
			dataByte >>= 1;
			bitCnt = (bitCnt + 1) & 0x07;
		}
	}

	int MbFrame::PutBitData(coil_t*  SrcBits, int SrcIdx, int FrameIdx)
	{
		SrcBits += SrcIdx;
		uint8_t * DestData = &RawData->Data[FrameIdx];

		int bitCnt = 8;
		uint8_t dataByte = 0;
		int NumBytes = 0;

		for (int i = 0; i < DataCount; ++i) {
			dataByte >>= 1;
			if (*SrcBits++ != 0) {
				dataByte |= 0x80;
			}

			if (--bitCnt == 0) {
				DestData[NumBytes] = dataByte;
				bitCnt = 8;
				dataByte = 0;
				++NumBytes;
			}
		}
		if (bitCnt != 0) {
			dataByte >>= bitCnt;
			DestData[NumBytes] = dataByte;
			++NumBytes;
		} else {
			bitCnt = 1;
		}
		RawData->Data[FrameIdx - 1] = (uint8_t)NumBytes;
		return NumBytes;
	}

	int MbFrame::ResponseMessageLength()
	{
		if (FunctionCode <= ModbusCodes::READ_INPUT_REGISTERS) {
			return 3;
		}

		switch (FunctionCode) {
		case ModbusCodes::WRITE_SINGLE_COIL:
		case ModbusCodes::WRITE_SINGLE_REGISTER:
		case ModbusCodes::WRITE_MULTIPLE_COILS:
		case ModbusCodes::WRITE_MULTIPLE_REGISTERS:
			return 6;
		default:
			// Should not happen Function code was 
			// already acepted in the receive message
			return 0;
		}
	}
}
