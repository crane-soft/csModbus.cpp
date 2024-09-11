
#include "Modbus/MbSlaveStateMachine.h"
#include "Interface/MbRTU.h"
#define SLAVE_ID	1
using namespace csModbusLib;

#define REGS_BASE_ADDR	10
#define MAX_REGS	8
uint16_t mod_registers[MAX_REGS];

class Nucleo_DataServer : public MbSlaveDataServer 
{
public:	
	Nucleo_DataServer() {}
	Nucleo_DataServer(int SlaveID) 
		:MbSlaveDataServer(SlaveID) {}
	
protected:			
    bool ReadHoldingRegisters() 
	{ 
		if (Frame->PutValues (REGS_BASE_ADDR, MAX_REGS, mod_registers)) {
			++mod_registers[0];
			return true; 
		}
		return false;
	}

	bool WriteSingleRegister() 
	{ 
		if (Frame->MatchAddress (REGS_BASE_ADDR, MAX_REGS)) {
			mod_registers[Frame->DataAddress - REGS_BASE_ADDR] = Frame->GetSingleUInt16();
			++mod_registers[0];
			return true; 
		}
		return false;
	}	
};

SerialSTM32				ModbusPort 		 = SerialSTM32(USART2,38400);
MbSlaveStateMachine 	ModbusSlave 	 = MbSlaveStateMachine();
MbRTU              		ModbusInterface  = MbRTU(&ModbusPort);
Nucleo_DataServer		ModbusDataServer = Nucleo_DataServer(SLAVE_ID);

extern "C"  {
void Init_Modbus()
{
	ModbusSlave.StartListen(&ModbusInterface, &ModbusDataServer);
}

void Handle_Modbus()
{
	//ModbusSlave.CheckStatus();
}

void USART2_IRQHandler(void)
{
	ModbusPort.IRQHandler();
}

}
