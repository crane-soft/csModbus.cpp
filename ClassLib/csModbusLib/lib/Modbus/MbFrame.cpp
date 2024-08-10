#include "MbBase.h"
#include "MbInterface.h"
#include "MbFrame.h"

#include <stdint.h>
#include <cstring>

namespace csModbusLib
{
	MbRawData::MbRawData()
	{
		Init(MbBase::MAX_FRAME_LEN);
	}

	MbRawData::MbRawData(int Size)
	{
		Init(Size);
	}
	void MbRawData::Init(int Size)
	{
		BuffSize = Size;
		Data = new uint8_t[Size];
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
		int maxLength = BuffSize - EndIdx;
		if (length > maxLength) {
			length = maxLength;
		}

		memcpy(&Data[EndIdx], &source[srcIdx], length);
		EndIdx += length;
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

	int MbRawData::CheckEthFrameLength()
	{
		int frameLength = GetUInt16(ADU_OFFS - 2);
		int bytesleft = (frameLength + ADU_OFFS) - EndIdx;
		return bytesleft;

	}

	uint8_t * MbRawData::GetBuffTail() {
		return &Data[EndIdx];
	}

	MbFrame::MbFrame()
	{
		ExceptionCode = ExceptionCodes::NO_EXCEPTION;
	}
	
	void MbFrame::GetBitData(coil_t * DestBits, int DestIdx, int FrameIdx)
	{
		DestBits += DestIdx;
		uint8_t * SrcByte = &RawData.Data[FrameIdx];

		int bitCnt = 0;
		uint8_t dataByte = 0;

		for (int i = 0; i < DataCount; ++i) {
			if (bitCnt == 0) {
				dataByte = *SrcByte++;
			}
			int b = dataByte & 1;
			*DestBits++ = (uint16_t)b;
			dataByte >>= 1;
			bitCnt = (bitCnt + 1) & 0x07;
		}
	}

	int MbFrame::PutBitData(coil_t*  SrcBits, int SrcIdx, int FrameIdx)
	{
		SrcBits += SrcIdx;
		uint8_t * DestData = &RawData.Data[FrameIdx];

		int bitCnt = 8;
		uint8_t dataByte = 0;
		int NumBytes = 0;

		for (int i = 0; i < DataCount; ++i) {
			dataByte >>= 1;
			if (*SrcBits++ != 0) {
				dataByte |= 0x80;
			}

			if (--bitCnt == 0) {
				DestData[NumBytes] = dataByte;
				bitCnt = 8;
				dataByte = 0;
				++NumBytes;
			}
		}
		if (bitCnt != 0) {
			dataByte >>= bitCnt;
			DestData[NumBytes] = dataByte;
			++NumBytes;
		} else {
			bitCnt = 1;
		}
		RawData.Data[FrameIdx - 1] = (uint8_t)NumBytes;
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
		default:
			throw ErrorCodes::ILLEGAL_FUNCTION_CODE;
		}
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

					// Create extra RawData for Write request
					WriteData = new MbRawData(REQST_DATA_IDX + AdditionalData);
					// Copy Head NodeID and Function Code
					WriteData->CopyFrom(RawData.Data, 0, REQST_ADDR_IDX);
					// Copy  the write data
					WriteData->CopyFrom(RawData.Data, REQST_WRADDR_IDX, AdditionalData + 5);

				} else {
					AdditionalData = RawData.Data[REQST_DATA_LEN_IDX];
				}
			}
		}
		return AdditionalData;

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

	uint16_t MBSFrame::GetRequestSingleUInt16()
	{
		return RawData.GetUInt16(REQST_SINGLE_DATA_IDX);
	}

	coil_t MBSFrame::GetRequestSingleBit()
	{
		return RawData.Data[REQST_SINGLE_DATA_IDX];
	}

	void MBSFrame::PutResponseBitValues(int BaseAddr, coil_t* SrcBits)
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
	
	void MBSFrame::GetRequestBitValues(int BaseAddr, coil_t* DestBits)
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
/*	void MBMFrame::SetSlaveID(uint8_t Slave_ID)
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
			Interface->ReceiveBytes(1);
			Bytes2Read = RawData.Data[RESPNS_LEN_IDX];
		} else {
			Bytes2Read = ResponseMessageLength() - 2;
		}
		if (Bytes2Read > 0)
			Interface->ReceiveBytes(Bytes2Read);
		Interface->EndOfFrame();
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
	} */
}
