#pragma once
#include "MbBase.h"

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
		int EndIdx;
		uint8_t *Data;

		MbRawData();
		MbRawData(int Size);

		void IniADUoffs();
		void CopyFrom(MbRawData *source);
		void CopyFrom(uint8_t *source, int srcIdx, int count);
		uint16_t GetUInt16(int ByteOffs);
		void PutUInt16(int ByteOffs, uint16_t Value);
		void CopyUInt16(uint16_t *DestArray, int SrcOffs, int DestOffs, int Length);
		void FillUInt16(uint16_t *SrcArray, int SrcOffs, int DestOffs, int Length);
	};

	class MbFrame {
	public:
		int SlaveId;
		ModbusCodes FunctionCode;
		uint16_t DataAddress;
		uint16_t DataCount;
		MbRawData RawData = MbRawData(MbBase::MAX_FRAME_LEN);
		ExceptionCodes ExceptionCode;

		MbFrame();
		void GetBitData(coil_t * DestBits, int DestIdx, int FrameIdx);
		int PutBitData(coil_t*  SrcBits, int SrcIdx, int FrameIdx);

	protected:
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

		int GetNumBytesOfBits();
		int ResponseMessageLength();
	};

	/* -------------------------------------------------------------------------------
	 * Modbus-Slave Frame
	 ---------------------------------------------------------------------------------*/
	class MBSFrame : public MbFrame
	{
	public:
		MBSFrame();
		void ReceiveMasterRequest(MbInterface *Interface);
		void GetRwWriteAddress();
		int ToMasterResponseMessageLength();
		bool MatchAddress(int BaseAddr, int Size);
		uint16_t GetRequestSingleUInt16();
		bool GetRequestSingleBit();
		void PutResponseBits(int BaseAddr, coil_t* SrcBits);
		void PutResponseValues(int BaseAddr, uint16_t * RegisterArray);
		void GetRequestValues(int BaseAddr, coil_t* DestBits);
		void GetRequestValues(int BaseAddr, uint16_t *DestArray);

	private:
		bool WrMultipleData;
		bool WrSingleData;
		bool ValidAddressFound;
		MbRawData *WriteData;
		int FromMasterRequestMessageLen();
		void ExceptionResponse(ExceptionCodes ErrorCode);
	};

	/* ------------------------------------------------------------
	 * Modbus Frame Master Functions
	 -------------------------------------------------------------- */
	class MBMFrame : public MbFrame
	{
	public:
		void SetSlaveID(uint8_t Slave_ID);
		int BuildRequest(ModbusCodes FuncCode, uint16_t Address, uint16_t DataOrLen);
		int BuildMultipleWriteRequest(ModbusCodes FuncCode, uint16_t Address, uint16_t Length, void * SrcData, int SrcOffs);
		int BuildMultipleReadWriteRequest(uint16_t RdAddress, uint16_t RdLength, uint16_t WrAddress, uint16_t WrLength, uint16_t* SrcData, int SrcOffs);
		void ReceiveSlaveResponse(MbInterface *Interface);
		void ReadSlaveRegisterValues(uint16_t* DestArray, int DestOffs);
		void ReadSlaveBitValues(coil_t* DestBits, int DestOffs);
		uint16_t GetTransactionIdentifier();
	private:
		static const int SLAVE_DATA_IDX = MbRawData::ADU_OFFS + 3;
		uint8_t Current_SlaveID;
	};
}

