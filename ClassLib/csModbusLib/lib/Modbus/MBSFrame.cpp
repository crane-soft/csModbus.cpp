#include "MBSFrame.h"

namespace csModbusLib
{
	MBSFrame::MBSFrame()
	{
		ValidAddressFound = false;
	}

	int MBSFrame::FromMasterRequestMessageLen()
	{
		WrSingleData = false;
		WrMultipleData = false;

		if (FunctionCode <= ModbusCodes::WRITE_SINGLE_REGISTER) {
			if (FunctionCode >= ModbusCodes::WRITE_SINGLE_COIL) {
				WrSingleData = true;
			}
			return 4;
		}

		switch (FunctionCode) {
		case ModbusCodes::WRITE_MULTIPLE_COILS:
		case ModbusCodes::WRITE_MULTIPLE_REGISTERS:
			WrMultipleData = true;
			return 5;
		case ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS:
			WrMultipleData = true;
			return 9;
		default:
			throw ErrorCodes::ILLEGAL_FUNCTION_CODE;
		}

	}

	int MBSFrame::ParseMasterRequest()
	{
		ExceptionCode = ExceptionCodes::NO_EXCEPTION;
		SlaveId = RawData.Data[REQST_UINIT_ID_IDX];
		FunctionCode = (ModbusCodes)RawData.Data[REQST_FCODE_IDX];
		int MsgLen = FromMasterRequestMessageLen();
		return MsgLen;
	}

	int MBSFrame::ParseDataCount()
	{
		DataAddress = RawData.GetUInt16(REQST_ADDR_IDX);

		int AdditionalData = 0;

		if (WrSingleData == true) {
			DataCount = 1;
		} else {
			DataCount = RawData.GetUInt16(REQST_DATA_CNT_IDX);
			if (WrMultipleData) {
				if (FunctionCode == ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS) {
					AdditionalData = RawData.Data[REQST_DATA_LEN_IDX + 4];
				} else {
					AdditionalData = RawData.Data[REQST_DATA_LEN_IDX];
				}
			}
		}
		return AdditionalData;

	}

	void MBSFrame::SaveWritaData()
	{
		int WrDataLen = RawData.Data[REQST_DATA_LEN_IDX + 4];
		// Copy Head NodeID and Function Code
		WriteData.CopyFrom(RawData.Data, 0, REQST_ADDR_IDX);
		// Copy  the write data
		WriteData.CopyFrom(RawData.Data, REQST_WRADDR_IDX, WrDataLen + 5);

	}
	void MBSFrame::ReceiveMasterRequest(MbInterface *Interface)
	{
		int MsgLen = ParseMasterRequest();
		Interface->ReceiveBytes(MsgLen);

		int AdditionalData = ParseDataCount();
		if (AdditionalData != 0) {
			Interface->ReceiveBytes(AdditionalData);
		}

		Interface->EndOfFrame();
	
	}

	void MBSFrame::GetRwWriteAddress()
	{
		// Write Address for READ_WRITE_MULTIPLE_REGISTERS
		DataAddress = WriteData.GetUInt16(REQST_ADDR_IDX);
		DataCount = WriteData.GetUInt16(REQST_DATA_CNT_IDX);
	}


	void MBSFrame::ExceptionResponse(ExceptionCodes ErrorCode)
	{
		RawData.Data[REQST_FCODE_IDX] |= 0x80;
		RawData.Data[RESPNS_ERR_IDX] = (uint8_t)ErrorCode;
	}


	int MBSFrame::ToMasterResponseMessageLength()
	{
		if (ExceptionCode == ExceptionCodes::NO_EXCEPTION) {
			if (ValidAddressFound == false) {
				ExceptionCode = ExceptionCodes::ILLEGAL_DATA_ADDRESS;
			}
		}

		if (ExceptionCode != ExceptionCodes::NO_EXCEPTION) {
			ExceptionResponse(ExceptionCode);
			return 3;
		}

		if ((FunctionCode <= ModbusCodes::READ_INPUT_REGISTERS) || (FunctionCode == ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS)) {
			return 3 + RawData.Data[RESPNS_LEN_IDX];
		} else {
			return ResponseMessageLength();
		}
	}

	bool MBSFrame::MatchAddress(int BaseAddr, int Size)
	{
		int EndAddr = BaseAddr + Size;
            // TODO wenn Size > als Array Fehler erzeugen
		if ((DataAddress >= BaseAddr) && (DataAddress < EndAddr)) {
			if ((DataAddress + DataCount - 1) < EndAddr) {
				ValidAddressFound = true;
				return true;
			}
		}
		return false;
	}

	uint16_t MBSFrame::GetSingleUInt16()
	{
		return RawData.GetUInt16(REQST_SINGLE_DATA_IDX);
	}

	coil_t MBSFrame::GetSingleBit()
	{
		return RawData.Data[REQST_SINGLE_DATA_IDX];
	}

	void MBSFrame::PutBitValues(int BaseAddr, coil_t* SrcBits)
	{
		PutBitData(SrcBits, DataAddress - BaseAddr, RESPNS_DATA_IDX);
	}

	void MBSFrame::PutValues(int BaseAddr, uint16_t * RegisterArray)
	{
		for (int i = 0; i < DataCount; ++i) {
			uint16_t Value = RegisterArray[DataAddress - BaseAddr + i];
			RawData.PutUInt16(RESPNS_DATA_IDX + i * 2, Value);

		}
		RawData.Data[RESPNS_LEN_IDX] = (uint8_t)(DataCount * 2);
	}
	
	void MBSFrame::GetBitValues(int BaseAddr, coil_t* DestBits)
	{
		GetBitData(DestBits, DataAddress - BaseAddr, REQST_DATA_IDX);
	}

	void MBSFrame::GetValues(int BaseAddr, uint16_t *DestArray)
	{
		MbRawData *SrcData;
		if (FunctionCode == ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS) {
			SrcData = &WriteData;
		} else {
			SrcData = &RawData;
		}
		SrcData->CopyUInt16(DestArray, REQST_DATA_IDX, DataAddress - BaseAddr, DataCount);
	}
}
