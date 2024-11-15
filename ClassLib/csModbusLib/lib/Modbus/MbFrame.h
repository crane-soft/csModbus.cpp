#pragma once
#include "platform.h"
#include "Modbus/MbBase.h"

/* Modbus Format
   Reuest Format
   READ_COILS = 0x01,               SlaveID(1)  FCode(1)    Address(2)  Count(2)
   READ_DISCRETE_INPUTS = 0x02,     SlaveID(1)  FCode(1)    Address(2)  Count(2)
   READ_HOLDING_REGISTERS = 0x03,   SlaveID(1)  FCode(1)    Address(2)  Count(2)
   READ_INPUT_REGISTERS = 0x04,     SlaveID(1)  FCode(1)    Address(2)  Count(2)
  
   WRITE_SINGLE_COIL = 0x05,        SlaveID(1)  FCode(1)    Address(2)  Value(2)
   WRITE_SINGLE_REGISTER = 0x06,    SlaveID(1)  FCode(1)    Address(2)  Value(2)
   READ_EXCEPTION_STATUS = 0x07,    -----------------

   WRITE_MULTIPLE_COILS = 0x0F,     SlaveID(1)  FCode(1)    Address(2)  Count(2)    NumBytes(1)  Data
   WRITE_MULTIPLE_REGISTERS = 0x10, SlaveID(1)  FCode(1)    Address(2)  Count(2)    NumBytes(1)  Data
   REPORT_SLAVE_ID = 0x11,
   READ_WRITE_MULTIPLE_REGS = 0x17, SlaveID(1)  FCode(1) 	RdAddr(2)	RdCnt(2)	WrAddr(2)	WrCnd(2)	NumBytes(1)  Data
 

   Respones Format
   READ_COILS = 0x01,               SlaveID(1)  FCode(1)    NumBytes(1)  Data
   READ_DISCRETE_INPUTS = 0x02,     SlaveID(1)  FCode(1)    NumBytes(1)  Data
   READ_HOLDING_REGISTERS = 0x03,   SlaveID(1)  FCode(1)    NumBytes(1)  Data
   READ_INPUT_REGISTERS = 0x04,     SlaveID(1)  FCode(1)    NumBytes(1)  Data
  
   WRITE_SINGLE_COIL = 0x05,        SlaveID(1)  FCode(1)    Address(2)  Value(2)
   WRITE_SINGLE_REGISTER = 0x06,    SlaveID(1)  FCode(1)    Address(2)  Value(2)
   READ_EXCEPTION_STATUS = 0x07,    -----------------

   WRITE_MULTIPLE_COILS = 0x0F,     SlaveID(1)  FCode(1)    Address(2)  Count(2)   
   WRITE_MULTIPLE_REGISTERS = 0x10, SlaveID(1)  FCode(1)    Address(2)  Count(2)   
   READ_WRITE_MULTIPLE_REGS = 0x17	SlaveID(1)  FCode(1)    NumBytes(1)  Data
   Exception Response               SlaveID(1)  FCode(1 	ExexptionCode(1)
 
 */

namespace csModbusLib
{
	class MbRawData
	{
	public:
		static const int ADU_OFFS = 6;
		int DataSize;
		int EndIdx;
		uint8_t *Data;

		//MbRawData();
		//MbRawData(int Size);
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

	class MbFrame {
	public:
		int SlaveId;
		ModbusCodes FunctionCode;
		uint16_t DataAddress;
		uint16_t DataCount;
		MbRawDataBuff<MbBase::MAX_FRAME_LEN> RawData;
		ExceptionCodes ExceptionCode;

		MbFrame();

	protected:
		void GetBitData(coil_t* DestBits, int DestIdx, int FrameIdx);
		int PutBitData(coil_t* SrcBits, int SrcIdx, int FrameIdx);

		static const int REQST_UINIT_ID_IDX = MbRawData::ADU_OFFS + 0;
		static const int REQST_FCODE_IDX = MbRawData::ADU_OFFS + 1;
		static const int RESPNS_ERR_IDX = MbRawData::ADU_OFFS + 2;

		static const int REQST_ADDR_IDX = MbRawData::ADU_OFFS + 2;
		static const int REQST_SINGLE_DATA_IDX = MbRawData::ADU_OFFS + 4;
		static const int REQST_DATA_CNT_IDX = MbRawData::ADU_OFFS + 4;   // Request for Single Write Functions
		static const int REQST_DATA_LEN_IDX = MbRawData::ADU_OFFS + 6;
		static const int REQST_WRADDR_IDX = MbRawData::ADU_OFFS + 6;

		static const int REQST_DATA_IDX = MbRawData::ADU_OFFS + 7;      // 
		static const int RESPNS_DATA_IDX = MbRawData::ADU_OFFS + 3;           // Response for Read Functions
		static const int RESPNS_LEN_IDX = MbRawData::ADU_OFFS + 2;

		int ResponseMessageLength();
	};
}
