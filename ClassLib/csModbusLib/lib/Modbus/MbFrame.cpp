#include "MbBase.h"
#include "MbInterface.h"
#include "MbFrame.h"

#include <stdint.h>
#include <cstring>

namespace csModbusLib
{
	MbRawData::MbRawData()
	{
		Data = 0;
		EndIdx = 0;
	}

	MbRawData::MbRawData(int Size)
	{
		Data = new uint8_t[Size];
		EndIdx = 0;
	}

	void MbRawData::IniADUoffs()
	{
		EndIdx = ADU_OFFS;
	}

	void MbRawData::CopyFrom(MbRawData *source)
	{
		memcpy(Data, source->Data, source->EndIdx);
		EndIdx = source->EndIdx;
	}

	void MbRawData::CopyFrom(uint8_t *source, int srcIdx, int count)
	{
		memcpy(&Data[EndIdx], &source[srcIdx], count);
		EndIdx += count;
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
			PutUInt16(DestOffs + i * 2, SrcArray[i]);
	}

	MbFrame::MbFrame()
	{
		ExceptionCode = ExceptionCodes::NO_EXCEPTION;
	}

	int MbFrame::GetNumBytesOfBits()
	{
		int numBytes = DataCount / 8;
		if ((DataCount % 8) > 0)
			++numBytes;
		return numBytes;
	}

	void MbFrame::GetBitData(coil_t * DestBits, int DestIdx, int FrameIdx)
	{
		DestBits += DestIdx;
		uint8_t * SrcByte = &RawData.Data[FrameIdx];
		uint16_t BitCount = DataCount;
		int NumBytes = GetNumBytesOfBits();
		for (int byte = 0; byte < NumBytes; ++byte) {
			uint8_t BitMask = 1;
			for (int bitNo = 0; bitNo < 8; ++bitNo) {
				*DestBits = (*SrcByte & BitMask) != 0;
				++DestBits;
				BitMask <<= 1;
				if (--BitCount == 0)
					break;
			}
			++SrcByte;
		}
	}

	int MbFrame::PutBitData(coil_t*  SrcBits, int SrcIdx, int FrameIdx)
	{
		SrcBits += SrcIdx;
		uint8_t * DestData = &RawData.Data[FrameIdx];
		uint16_t BitCount = DataCount;
		int NumBytes = GetNumBytesOfBits();
		for (int byte = 0; byte < NumBytes; ++byte) {
			uint8_t BitMask = 1;
			uint8_t FrameByte = *DestData;
			for (int bit = 0; bit < 8; ++bit) {
				FrameByte = FrameByte & ~BitMask;
				if (*SrcBits != 0)
					FrameByte |= BitMask;
				++SrcBits;
				BitMask <<= 1;
				if (--BitCount == 0)
					break;
			}
			*DestData = FrameByte;
			++DestData;
		}

		RawData.Data[FrameIdx - 1] = (uint8_t)(NumBytes);
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
		}
		throw ErrorCodes::ILLEGAL_FUNCTION_CODE;
	}


	/* -------------------------------------------------------------------------------
	 * Modbus-Slave Frame
	 ---------------------------------------------------------------------------------*/
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
		}

		throw ErrorCodes::ILLEGAL_FUNCTION_CODE;
	}

	void MBSFrame::ReceiveMasterRequest(MbInterface *Interface)
	{
		ExceptionCode = ExceptionCodes::NO_EXCEPTION;

		SlaveId = RawData.Data[REQST_UINIT_ID_IDX];
		FunctionCode = (ModbusCodes)RawData.Data[REQST_FCODE_IDX];

		int MsgLen = FromMasterRequestMessageLen();
		Interface->ReceiveBytes(&RawData, MsgLen);

		DataAddress = RawData.GetUInt16(REQST_ADDR_IDX);

		if (WrSingleData == true) {
			DataCount = 1;
		} else {
			DataCount = RawData.GetUInt16(REQST_DATA_CNT_IDX);
			if (WrMultipleData) {
				int DataLength;
				if (FunctionCode == ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS) {
					DataLength = RawData.Data[REQST_DATA_LEN_IDX + 4];

					// Create extra RawData for Write request
					WriteData = new MbRawData(REQST_DATA_IDX + DataLength);
					// Copy Head NodeID and Function Code
					WriteData->CopyFrom(RawData.Data, 0, REQST_ADDR_IDX);
					// Copy  the write data
					WriteData->CopyFrom(RawData.Data, REQST_WRADDR_IDX, DataLength + 5);

				} else {
					DataLength = RawData.Data[REQST_DATA_LEN_IDX];
					Interface->ReceiveBytes(&RawData, DataLength);
				}
			}
		}

		Interface->EndOfFrame(&RawData);
	}

	void MBSFrame::GetRwWriteAddress()
	{
		// Write Address for READ_WRITE_MULTIPLE_REGISTERS
		DataAddress = WriteData->GetUInt16(REQST_ADDR_IDX);
		DataCount = WriteData->GetUInt16(REQST_DATA_CNT_IDX);
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
		}
		else {
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

	uint16_t MBSFrame::GetRequestSingleUInt16()
	{
		return RawData.GetUInt16(REQST_SINGLE_DATA_IDX);
	}

	bool MBSFrame::GetRequestSingleBit()
	{
		return RawData.Data[REQST_SINGLE_DATA_IDX] != 0;
	}

	void MBSFrame::PutResponseBits(int BaseAddr, coil_t* SrcBits)
	{
		PutBitData(SrcBits, DataAddress - BaseAddr, RESPNS_DATA_IDX);
	}

	void MBSFrame::PutResponseValues(int BaseAddr, uint16_t * RegisterArray)
	{
		for (int i = 0; i < DataCount; ++i) {
			uint16_t Value = RegisterArray[DataAddress - BaseAddr + i];
			RawData.PutUInt16(RESPNS_DATA_IDX + i * 2, Value);

		}
		RawData.Data[RESPNS_LEN_IDX] = (uint8_t)(DataCount * 2);
	}
	
	void MBSFrame::GetRequestValues(int BaseAddr, coil_t* DestBits)
	{
		GetBitData(DestBits, DataAddress - BaseAddr, REQST_DATA_IDX);
	}

	void MBSFrame::GetRequestValues(int BaseAddr, uint16_t *DestArray)
	{
		MbRawData *SrcData;
		if (FunctionCode == ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS) {
			SrcData = WriteData;
		} else {
			SrcData = &RawData;
		}
		SrcData->CopyUInt16(DestArray, REQST_DATA_IDX, DataAddress - BaseAddr, DataCount);
	}

	/* ------------------------------------------------------------
	 * Modbus Frame Master Functions
	 -------------------------------------------------------------- */
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
			NumDataBytes = Length * 2; // TODO überflow
			RawData.Data[REQST_DATA_LEN_IDX] = (uint8_t)NumDataBytes;
			RawData.FillUInt16((uint16_t*)SrcData, SrcOffs, REQST_DATA_IDX, Length);
		}
		else if (FuncCode == ModbusCodes::WRITE_MULTIPLE_COILS) {
			NumDataBytes = PutBitData((coil_t*)SrcData, SrcOffs, REQST_DATA_IDX);
		}
		else {
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
		FunctionCode = (ModbusCodes)RawData.Data[REQST_FCODE_IDX];

		int Bytes2Read;
		if ((FunctionCode <= ModbusCodes::READ_INPUT_REGISTERS) || (FunctionCode == ModbusCodes::READ_WRITE_MULTIPLE_REGISTERS)) {
			Interface->ReceiveBytes(&RawData, 1);
			Bytes2Read = RawData.Data[RESPNS_LEN_IDX];
		}
		else {
			Bytes2Read = ResponseMessageLength() - 2;
		}
		if (Bytes2Read > 0)
			Interface->ReceiveBytes(&RawData, Bytes2Read);
		Interface->EndOfFrame(&RawData);
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
