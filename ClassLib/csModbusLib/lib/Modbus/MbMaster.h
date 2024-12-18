#pragma once

#include "MbBase.h"
#include "MBMFrame.h"

namespace csModbusLib {
	class MbMasterBase : public MbBase {

	public:
		MbMasterBase();
		MbMasterBase(MbInterface *Interface);

		bool IsConnected();
		uint8_t Get_Slave_ID();
		void  Set_SlaveID(uint8_t value);
		ErrorCodes Get_ErrorCode();
		uint16_t Get_TransactionIdentifier();
		bool Connect();
		bool Connect(uint8_t newSlaveID);
		bool Connect(MbInterface *Interface, uint8_t newSlaveID);
		void Close();
		ExceptionCodes GetModusException();

	protected:
		ErrorCodes LastError;
		bool SendSingleRequest(ModbusCodes Fcode, uint16_t Address, uint16_t DataOrLen);
		bool SendMultipleWriteRequest(ModbusCodes Fcode, uint16_t Address, uint16_t Length, void * SrcData, int SrcOffs);
		bool SendMultipleReadWriteRequest(uint16_t RdAddress, uint16_t RdLength, uint16_t WrAddress, uint16_t WrLength, uint16_t* SrcData, int SrcOffs);
		bool SendRequestMessage(int MsgLen);
		bool ReadSlaveBitValues(coil_t* DestArray, int DestOffs);
		bool ReadSlaveRegisterValues(uint16_t* DestArray, int DestOffs);
		bool ReceiveSlaveResponse();

	private:
		void InitInterface(MbInterface* Interface);
		MBMFrame Frame = MBMFrame();
		uint8_t Current_SlaveID;

	};

	class MbMaster : public MbMasterBase {

	public: 
		MbMaster() {}
		MbMaster(MbInterface *Interface) : MbMasterBase(Interface){}

		ErrorCodes ReadCoils(uint16_t Address, uint16_t Length, coil_t* DestData, int DestOffs = 0);
		ErrorCodes ReadDiscreteInputs(uint16_t Address, uint16_t Length, coil_t* DestData, int DestOffs = 0);
		ErrorCodes ReadHoldingRegisters(uint16_t Address, uint16_t Length, uint16_t* DestData, int DestOffs = 0);
		ErrorCodes ReadInputRegisters(uint16_t Address, uint16_t Length, uint16_t* DestData, int DestOffs = 0);
		ErrorCodes WriteSingleCoil(uint16_t Address, coil_t BitData);
		ErrorCodes WriteSingleRegister(uint16_t Address, uint16_t Data);
		ErrorCodes WriteMultipleCoils(uint16_t Address, uint16_t Length, coil_t* SrcData, int SrcOffs = 0);
		ErrorCodes WriteMultipleRegisters(uint16_t Address, uint16_t Length, uint16_t* SrcData, int SrcOffs = 0);
		ErrorCodes ReadWriteMultipleRegisters(uint16_t RdAddress, uint16_t RdLength, uint16_t* DestData,
								uint16_t WrAddress, uint16_t WrLength, uint16_t* SrcData, int DestOffs = 0, int SrcOffs = 0);
	};
}
