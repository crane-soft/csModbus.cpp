#pragma once
#include "platform.h"
#include "Modbus/MbBase.h"

namespace csModbusLib
{
	class MbRawData
	{
	public:

		MbRawData(uint8_t* Buffer, int Size);
		void Init(uint8_t* Buffer, int Size);

		void Clear();
		void CopyFrom(uint8_t *source, int srcIdx, int length);

		void PutUInt16(int ByteOffs, uint16_t Value);
		uint16_t GetUInt16(int ByteOffs) const;

		void CopyUInt16(uint16_t *DestArray, int SrcOffs, int DestOffs, int Length);
		void FillUInt16(uint16_t *SrcArray, int SrcOffs, int DestOffs, int Length);

		uint8_t* BuffStart() const;
		uint8_t * BufferEnd() const;
		uint8_t LastByte() const;

		uint8_t* Data;
		int Length;

	protected:
		int DataSize;
	};

	class MbEthData : public MbRawData
	{
	public:
		MbEthData(uint8_t* Buffer, int Size);
		int CheckEthFrameLength(int readed);
		void FillMBAPHeader(int Length, uint16_t TransactionIdentifier);
		void CopyFrom(MbEthData* source);
		uint8_t* EthData;
		int EthSize;

	private:
	};

	template  <int const BufferSize>
	class MbRawDataBuff : public MbRawData
	{
	public:
		MbRawDataBuff() : MbRawData(Buffer, BufferSize) {}
	private:
		uint8_t Buffer[BufferSize];
	};

	template  <int const BufferSize>
	class MbEthDataBuff : public MbEthData
	{
	public:
		MbEthDataBuff() : MbEthData(Buffer, BufferSize+6)  	{}
	private:
		uint8_t Buffer[BufferSize +6];
	};


}
