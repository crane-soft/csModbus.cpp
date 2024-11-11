#include "MBMFrame.h"
#include "Interface/MbInterface.h"

namespace csModbusLib
{
	void MBMFrame::SetSlaveID(uint8_t Slave_ID)
	{
		Current_SlaveID = Slave_ID;
	}

	int MBMFrame::BuildRequest(ModbusCodes FuncCode, uint16_t Address, uint16_t DataOrLen)
	{
		DataAddress = Address;
		DataCount = DataOrLen;   // Data kan sein DataCount oder Single Data
		RawData.Data[REQST_UINIT_ID_IDX] = Current_SlaveID;
		RawData.Data[REQST_FCODE_IDX] = (uint8_t)FuncCode;
		RawData.PutUInt16(REQST_ADDR_IDX, Address);
		RawData.PutUInt16(REQST_SINGLE_DATA_IDX, DataOrLen);
		return 6;
	}

	int MBMFrame::BuildMultipleWriteRequest(ModbusCodes FuncCode, uint16_t Address, uint16_t Length, void * SrcData, int SrcOffs)
	{
		BuildRequest(FuncCode, Address, Length);
		int NumDataBytes;

		if (FuncCode == ModbusCodes::WRITE_MULTIPLE_REGISTERS) {
			NumDataBytes = Length * 2; // TODO overflow
			RawData.Data[REQST_DATA_LEN_IDX] = (uint8_t)NumDataBytes;
			RawData.FillUInt16((uint16_t*)SrcData, SrcOffs, REQST_DATA_IDX, Length);
		} else if (FuncCode == ModbusCodes::WRITE_MULTIPLE_COILS) {
			NumDataBytes = PutBitData((coil_t*)SrcData, SrcOffs, REQST_DATA_IDX);
		} else {
			return 0;
		}
		return 7 + NumDataBytes;
	}

	int MBMFrame::BuildMultipleReadWriteRequest(uint16_t RdAddress, uint16_t RdLength, uint16_t WrAddress, uint16_t WrLength, uint16_t* SrcData, int SrcOffs)
	{
		BuildRequest(ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS, RdAddress, RdLength);
		RawData.PutUInt16(REQST_ADDR_IDX + 4, WrAddress);
		RawData.PutUInt16(REQST_DATA_CNT_IDX + 4, WrLength);

		int NumDataBytes = WrLength * 2; // TODO overflow
		RawData.Data[REQST_DATA_LEN_IDX + 4] = (uint8_t)NumDataBytes;
		RawData.FillUInt16(SrcData, SrcOffs, REQST_DATA_IDX + 4, WrLength);
		return 11 + NumDataBytes;
	}

	void MBMFrame::ReceiveSlaveResponse(MbInterface *Interface)
	{
		SlaveId = RawData.Data[REQST_UINIT_ID_IDX];
		uint8_t RetCode = RawData.Data[REQST_FCODE_IDX];
		if ((RetCode & 0x80) != 0) {
			// Slave reports exception error
			Interface->ReceiveBytes(1);
			ExceptionCode = (ExceptionCodes)RawData.Data[RESPNS_ERR_IDX];
			throw ErrorCodes::MODBUS_EXCEPTION;
		}

		FunctionCode = (ModbusCodes)RetCode;

		int Bytes2Read;
		if ((FunctionCode <= ModbusCodes::READ_INPUT_REGISTERS) || (FunctionCode == ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS)) {
			Interface->ReceiveBytes(1);	// Need num bytes
			Bytes2Read = RawData.Data[RESPNS_LEN_IDX];
		} else {
			Bytes2Read = ResponseMessageLength() - 2;
		}
		if (Bytes2Read > 0)
			Interface->ReceiveBytes(Bytes2Read);
		if (Interface->EndOfFrame() == false)
			throw ErrorCodes::WRONG_CRC;
	}

	void MBMFrame::ReadSlaveRegisterValues(uint16_t* DestArray, int DestOffs)
	{
		RawData.CopyUInt16(DestArray, SLAVE_DATA_IDX, DestOffs, DataCount);
	}

	void MBMFrame::ReadSlaveBitValues(coil_t* DestBits, int DestOffs)
	{
		GetBitData(DestBits, DestOffs, SLAVE_DATA_IDX);
	}

	uint16_t MBMFrame::GetTransactionIdentifier()
	{
		return RawData.GetUInt16(0);
	} 
}
