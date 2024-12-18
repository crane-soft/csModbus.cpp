#pragma once
#include "platform.h"
#include "Modbus/MbBase.h"

namespace csModbusLib
{
	class MbRawData
	{
	public:
		union unFrameBuff {
			struct {
				uint8_t ADT[6];
				uint8_t Data[256];
			} pFrame;
			uint8_t pData[256 + 6];
		};
		static const int ADU_OFFS = 6;
		int DataSize;
		int EndIdx;
		uint8_t *Data;

		MbRawData(uint8_t* Data, int Size);
		void Init(uint8_t* Data, int Size);

		void Clear();
		void CopyFrom(MbRawData *source);
		void CopyFrom(uint8_t *source, int srcIdx, int length);
		uint16_t GetUInt16(int ByteOffs) const;
		void PutUInt16(int ByteOffs, uint16_t Value);
		void CopyUInt16(uint16_t *DestArray, int SrcOffs, int DestOffs, int Length);
		void FillUInt16(uint16_t *SrcArray, int SrcOffs, int DestOffs, int Length);
		int CheckEthFrameLength() const;
		
		uint8_t* DataStart() const;
		uint8_t * BufferEnd() const;
		int Length() const;
		uint8_t LastByte() const;
	};

	template  <int const BuffSize>
	class MbRawDataBuff : public MbRawData
	{
	public:
		MbRawDataBuff() : MbRawData(Buffer, BuffSize) {}
	private:

		uint8_t Buffer[BuffSize];
	};
}
