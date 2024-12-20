#pragma once
#include "platform.h"
#include "Modbus/MbBase.h"
#include "Modbus/MbRawData.h"

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
	class MbFrame {
	public:
		int SlaveId;
		ModbusCodes FunctionCode;
		uint16_t DataAddress;
		uint16_t DataCount;
		//MbRawDataBuff<MbBase::MAX_FRAME_LEN> RawData;
		MbRawData* RawData;
		ExceptionCodes ExceptionCode;

		MbFrame();

	protected:
		void GetBitData(coil_t* DestBits, int DestIdx, int FrameIdx);
		int PutBitData(coil_t* SrcBits, int SrcIdx, int FrameIdx);

		static const int REQST_UINIT_ID_IDX = 0;
		static const int REQST_FCODE_IDX = 1;
		static const int RESPNS_ERR_IDX = 2;

		static const int REQST_ADDR_IDX = 2;
		static const int REQST_SINGLE_DATA_IDX = 4;
		static const int REQST_DATA_CNT_IDX = 4;   // Request for Single Write Functions
		static const int REQST_DATA_LEN_IDX = 6;
		static const int REQST_WRADDR_IDX = 6;

		static const int REQST_DATA_IDX = 7;      // 
		static const int RESPNS_DATA_IDX = 3;           // Response for Read Functions
		static const int RESPNS_LEN_IDX = 2;

		static const int SLAVE_DATA_IDX = 3;

		int ResponseMessageLength();
	};
}
