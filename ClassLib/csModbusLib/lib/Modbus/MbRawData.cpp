#include "Modbus/MbRawData.h"
#include <cstring>

namespace csModbusLib
{
	MbRawData::MbRawData(uint8_t* Data, int Size)
	{
		Init(Data, Size);
	}

	void MbRawData::Init(uint8_t* Data, int Size)
	{
		this->Data = Data;
		DataSize = Size;
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
		int maxLength = DataSize - EndIdx;
		if (length > maxLength) {
			length = maxLength;
		}

		memcpy(&Data[EndIdx], &source[srcIdx], length);
		EndIdx += length;
	}

	uint16_t MbRawData::GetUInt16(int ByteOffs)  const
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

	int MbRawData::CheckEthFrameLength() const
	{
		int frameLength = GetUInt16(ADU_OFFS - 2);
		int bytesleft = (frameLength + ADU_OFFS) - EndIdx;
		return bytesleft;

	}

	uint8_t* MbRawData::DataStart() const
	{
		return &Data[MbRawData::ADU_OFFS];
	}

	uint8_t * MbRawData::BufferEnd() const {
		return &Data[EndIdx];
	}

	int MbRawData::Length() const
	{
		return (EndIdx - MbRawData::ADU_OFFS);
	}

	uint8_t MbRawData::LastByte() const
	{
		return Data[EndIdx-1];
	}
}
