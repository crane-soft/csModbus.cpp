#include "Modbus/MbRawData.h"
#include <cstring>

const int ADU_OFFS = 6;

namespace csModbusLib
{
	MbRawData::MbRawData(uint8_t* Buffer, int Size)
	{
		Init(Buffer, Size);
	}

	void MbRawData::Init(uint8_t* Buffer, int Size)
	{
		Data = Buffer;
		DataSize = Size;
		Length = 0;
	}

	void MbRawData::Clear()
	{
		Length = 0;
	}

	void MbRawData::CopyFrom(uint8_t *source, int srcIdx, int length)
	{
		int maxLength = DataSize - Length;
		if (length > maxLength) {
			length = maxLength;
		}

		memcpy(&Data[Length], &source[srcIdx], length);
		Length += length;
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

	uint8_t* MbRawData::BuffStart() const
	{
		return Data;
	}

	uint8_t * MbRawData::BufferEnd() const {
		return &Data[Length];
	}

	uint8_t MbRawData::LastByte() const
	{
		return Data[Length-1];
	}


	MbEthData::MbEthData(uint8_t* Buffer, int Size)
		: MbRawData (Buffer+6,Size-6)
	{
		EthData = Buffer;
		EthSize = Size;
	}

	int MbEthData::CheckEthFrameLength(int readed)
	{
		int frameLength = GetUInt16(-2);
		int bytesleft = (frameLength + 6) - readed;
		return bytesleft;
	}

	void MbEthData::FillMBAPHeader(int Length, uint16_t TransactionIdentifier)
	{
		PutUInt16(-6, TransactionIdentifier);
		PutUInt16(-4, 0);
		PutUInt16(-2, (uint16_t)Length);
	}

	void MbEthData::CopyFrom(MbEthData* source)
	{
		memcpy(EthData, source->EthData, source->Length);
		Length = source->Length;
	}
}
