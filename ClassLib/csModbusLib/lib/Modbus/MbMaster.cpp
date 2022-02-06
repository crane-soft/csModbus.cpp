#include "MbMaster.h"

namespace csModbusLib {

	MbMasterBase::MbMasterBase() {}

	MbMasterBase::MbMasterBase(MbInterface *Interface)
	{
		InitInterface(Interface);
	}

	bool MbMasterBase::IsConnected()
	{
		return running;
	}

	uint8_t MbMasterBase::Get_Slave_ID()
	{
		return Current_SlaveID;
	}

	void  MbMasterBase::Set_SlaveID(uint8_t value){
		Current_SlaveID = value;
		Frame.SetSlaveID(value);

	}

	ErrorCodes MbMasterBase::Get_ErrorCode()
	{
		return LastError;
	}

	uint16_t MbMasterBase::Get_TransactionIdentifier()
	{
		return Frame.GetTransactionIdentifier();
	}

	bool MbMasterBase::Connect()
	{
		if (gInterface != 0) {
			if (running) {
				Close();
			}
			if (gInterface->Connect()) {
				running = true;
				return true;
			}
		}
		return false;
	}

	bool MbMasterBase::Connect(uint8_t newSlaveID)
	{
		Set_SlaveID(newSlaveID);
		return Connect();
	}

	bool MbMasterBase::Connect(MbInterface *Interface, uint8_t newSlaveID)
	{
		InitInterface(Interface);
		Set_SlaveID(newSlaveID);
		return Connect();
	}

	void MbMasterBase::Close()
	{
		running = false;
		if (gInterface != 0) {
			gInterface->DisConnect();
		}
	}

	ExceptionCodes MbMasterBase::GetModusException()
	{
		return Frame.ExceptionCode;
	}

	bool MbMasterBase::SendSingleRequest(ModbusCodes Fcode, uint16_t Address, uint16_t DataOrLen)
	{
		int MsgLen = Frame.BuildRequest(Fcode, Address, DataOrLen);
		return SendRequestMessage(MsgLen);
	}

	bool MbMasterBase::SendMultipleWriteRequest(ModbusCodes Fcode, uint16_t Address, uint16_t Length, void * SrcData, int SrcOffs)
	{
		int MsgLen = Frame.BuildMultipleWriteRequest(Fcode, Address, Length, SrcData, SrcOffs);
		return SendRequestMessage(MsgLen);
	}

	bool MbMasterBase::SendMultipleReadWriteRequest(uint16_t RdAddress, uint16_t RdLength, uint16_t WrAddress, uint16_t WrLength, uint16_t* SrcData, int SrcOffs)
	{
		int MsgLen = Frame.BuildMultipleReadWriteRequest(RdAddress, RdLength, WrAddress, WrLength, SrcData, SrcOffs);
		return SendRequestMessage(MsgLen);
	}

	bool MbMasterBase::SendRequestMessage(int MsgLen)
	{   // TODO check if connected
		LastError = ErrorCodes::MB_NO_ERROR;
		try {
			gInterface->SendFrame(&Frame.RawData, MsgLen);
		} catch (ErrorCodes errCode) {
			LastError = errCode;
			if (running) {
				gInterface->ReConnect();
			}
			return false;
		} catch (int errCode) {
			LastError = ErrorCodes::CONNECTION_ERROR;
			return false;

		}
		return true;
	}

	bool MbMasterBase::ReadSlaveBitValues(coil_t* DestArray, int DestOffs)
	{
		if (ReceiveSlaveResponse()) {
			Frame.ReadSlaveBitValues(DestArray, DestOffs);
			return true;
		}
		return false;
	}

	bool MbMasterBase::ReadSlaveRegisterValues(uint16_t* DestArray, int DestOffs)
	{
		if (ReceiveSlaveResponse()) {
			Frame.ReadSlaveRegisterValues(DestArray, DestOffs);
			return true;
		}
		return false;
	}

	bool MbMasterBase::ReceiveSlaveResponse()
	{
		try {
			gInterface->ReceiveHeader(MbInterface::ResponseTimeout, &Frame.RawData);
			Frame.ReceiveSlaveResponse(gInterface);
		} catch (ErrorCodes errCode) {
			if ((errCode != ErrorCodes::CONNECTION_CLOSED) && (errCode != ErrorCodes::MODBUS_EXCEPTION))
				gInterface->ReConnect();
			LastError = errCode;
			return false;
		}
		return true;
	}

	ErrorCodes MbMaster::ReadCoils(uint16_t Address, uint16_t Length, coil_t* DestData, int DestOffs)
	{
		if (SendSingleRequest(ModbusCodes::READ_COILS, Address, Length))
			ReadSlaveBitValues(DestData, DestOffs);
		return LastError;
	}

	ErrorCodes MbMaster::ReadDiscreteInputs(uint16_t Address, uint16_t Length, coil_t* DestData, int DestOffs)
	{
		if (SendSingleRequest(ModbusCodes::READ_DISCRETE_INPUTS, Address, Length))
			ReadSlaveBitValues(DestData, DestOffs);
		return LastError;
	}

	ErrorCodes MbMaster::ReadHoldingRegisters(uint16_t Address, uint16_t Length, uint16_t* DestData, int DestOffs )
	{
		if (SendSingleRequest(ModbusCodes::READ_HOLDING_REGISTERS, Address, Length))
			ReadSlaveRegisterValues(DestData, DestOffs);
		return LastError;
	}

	ErrorCodes MbMaster::ReadInputRegisters(uint16_t Address, uint16_t Length, uint16_t* DestData, int DestOffs)
	{
		if (SendSingleRequest(ModbusCodes::READ_INPUT_REGISTERS, Address, Length))
			ReadSlaveRegisterValues(DestData, DestOffs);
		return LastError;
	}

	ErrorCodes MbMaster::WriteSingleCoil(uint16_t Address, coil_t BitData)
	{
		uint16_t Data = (uint16_t)(BitData ? 0xff00 : 0);
		if (SendSingleRequest(ModbusCodes::WRITE_SINGLE_COIL, Address, Data))
			ReceiveSlaveResponse();
		return LastError;
	}

	ErrorCodes MbMaster::WriteSingleRegister(uint16_t Address, uint16_t Data)
	{
		if (SendSingleRequest(ModbusCodes::WRITE_SINGLE_REGISTER, Address, Data))
			ReceiveSlaveResponse();
		return LastError;
	}

	ErrorCodes MbMaster::WriteMultipleCoils(uint16_t Address, uint16_t Length, coil_t* SrcData, int SrcOffs)
	{   // TODO not yet tested
		if (SendMultipleWriteRequest(ModbusCodes::WRITE_MULTIPLE_COILS, Address, Length, SrcData, SrcOffs))
			ReceiveSlaveResponse();
		return LastError;
	}

	ErrorCodes MbMaster::WriteMultipleRegisters(uint16_t Address, uint16_t Length, uint16_t* SrcData, int SrcOffs)
	{  // TODO not yet tested
		if (SendMultipleWriteRequest(ModbusCodes::WRITE_MULTIPLE_REGISTERS, Address, Length, SrcData, SrcOffs))
			ReceiveSlaveResponse();

		return LastError;
	}

	ErrorCodes MbMaster::ReadWriteMultipleRegisters(uint16_t RdAddress, uint16_t RdLength, uint16_t* DestData,
		uint16_t WrAddress, uint16_t WrLength, uint16_t* SrcData, int DestOffs, int SrcOffs)
	{  // TODO not yet tested
		if (SendMultipleReadWriteRequest(RdAddress, RdLength, WrAddress, WrLength, SrcData, SrcOffs))
			ReadSlaveRegisterValues(DestData, DestOffs);
		return LastError;
	}
}
