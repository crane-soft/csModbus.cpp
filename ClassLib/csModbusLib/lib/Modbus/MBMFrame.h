#pragma once
#include "MbFrame.h"

namespace csModbusLib
{
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

