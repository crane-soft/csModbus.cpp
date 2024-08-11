#include "MbFrame.h"
#include <cstring>

namespace csModbusLib
{
	MbRawData::MbRawData()
	{
		Init(MbBase::MAX_FRAME_LEN);
	}

	MbRawData::MbRawData(int Size)
	{
		Init(Size);
	}
	void MbRawData::Init(int Size)
	{
		BuffSize = Size;
		Data = new uint8_t[Size];
		EndIdx = 0;

	}
	void MbRawData::Clear()
	{
		EndIdx = ADU_OFFS;
	}

	void MbRawData::CopyFrom(MbRawData *source)
	{
		memcpy(Data, source->Data, source->EndIdx);
		EndIdx = source->EndIdx;
	}

	void MbRawData::CopyFrom(uint8_t *source, int srcIdx, int length)
	{
		int maxLength = BuffSize - EndIdx;
		if (length > maxLength) {
			length = maxLength;
		}

		memcpy(&Data[EndIdx], &source[srcIdx], length);
		EndIdx += length;
	}

	uint16_t MbRawData::GetUInt16(int ByteOffs)
	{
		return (uint16_t)((Data[ByteOffs] << 8) | (Data[ByteOffs + 1] & 0x00FF));
	}

	void MbRawData::PutUInt16(int ByteOffs, uint16_t Value)
	{
		Data[ByteOffs] = (uint8_t)(Value >> 8);
		Data[ByteOffs + 1] = (uint8_t)(Value & 0x00FF);
	}

	void MbRawData::CopyUInt16(uint16_t *DestArray, int SrcOffs, int DestOffs, int Length)
	{
		for (int i = 0; i < Length; ++i)
			DestArray[DestOffs + i] = GetUInt16(SrcOffs + i * 2);
	}

	void MbRawData::FillUInt16(uint16_t *SrcArray, int SrcOffs, int DestOffs, int Length)
	{
		for (int i = 0; i < Length; ++i)
			PutUInt16(DestOffs + i * 2, SrcArray[SrcOffs+i]);
	}

	int MbRawData::CheckEthFrameLength()
	{
		int frameLength = GetUInt16(ADU_OFFS - 2);
		int bytesleft = (frameLength + ADU_OFFS) - EndIdx;
		return bytesleft;

	}

	uint8_t * MbRawData::GetBuffTail() {
		return &Data[EndIdx];
	}

	MbFrame::MbFrame()
	{
		ExceptionCode = ExceptionCodes::NO_EXCEPTION;
	}
	
	void MbFrame::GetBitData(coil_t * DestBits, int DestIdx, int FrameIdx)
	{
		DestBits += DestIdx;
		uint8_t * SrcByte = &RawData.Data[FrameIdx];

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
		uint8_t * DestData = &RawData.Data[FrameIdx];

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
		RawData.Data[FrameIdx - 1] = (uint8_t)NumBytes;
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
			throw ErrorCodes::ILLEGAL_FUNCTION_CODE;
		}
	}
}
