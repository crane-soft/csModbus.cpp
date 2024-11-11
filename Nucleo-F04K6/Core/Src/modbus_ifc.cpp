
#include "Modbus/MbRtuSlaveStm.h"
#include "SlaveDataServer/MbSlaveEmbServer.h"
#include "Gpio.h"

#define SLAVE_ID	1
using namespace csModbusLib;

#define REGS_BASE_ADDR	10
#define MAX_REGS	8
uint16_t mod_registers[MAX_REGS];

class Nucleo_DataServer : public MbSlaveEmbServer
{
public:	
	Nucleo_DataServer(int SlaveID)
		: MbSlaveEmbServer(SlaveID) {}
	
protected:			
    bool ReadHoldingRegisters() override
	{ 
		if (Frame->PutValues (REGS_BASE_ADDR, MAX_REGS, mod_registers)) {
			++mod_registers[0];
			return true; 
		}
		return false;
	}

	bool WriteSingleRegister() override
	{ 
		if (Frame->MatchAddress (REGS_BASE_ADDR, MAX_REGS)) {
			mod_registers[Frame->DataAddress - REGS_BASE_ADDR] = Frame->GetSingleUInt16();
			++mod_registers[1];
			return true; 
		}
		return false;
	}	

	coil_t * ReadCoils(uint16_t Address, int Size) override
    {
		if ((Address == 10) && (Size == 1)) {
			return &LD3_Status;
		}
    	return 0;
    }

	bool WriteSingleCoil(uint16_t Address, coil_t Bit) override
	{
		if (Address == 10) {
			LD3_Status = Bit;
			HAL_GPIO_WritePin (LD3_GPIO_Port, LD3_Pin, (GPIO_PinState) Bit);
			return true;
		}
		return false;
	}
private:
	coil_t LD3_Status = 0;
};

//#define MB_SERVER

SerialSTM32				ModbusPort 		 = SerialSTM32(USART2,9600);
#ifdef MB_SERVER
MbRTU					ModbusInterfave  = MbRTU(&ModbusPort);
MbSlave					ModbusSlave 	 = MbSlave (&ModbusInterfave);
#else
MbRtuSlaveStm			ModbusSlave 	 = MbRtuSlaveStm (&ModbusPort);
#endif
Nucleo_DataServer		ModbusDataServer = Nucleo_DataServer(SLAVE_ID);

extern "C"  {
void Init_Modbus()
{
	ModbusSlave.StartListen(&ModbusDataServer);
#ifdef MB_SERVER
	while (1)
		ModbusSlave.HandleRequestMessages();
#endif
}

void USART2_IRQHandler(void)
{
	ModbusPort.IRQHandler();
}

void EXTI4_15_IRQHandler(void)
{
	ModbusPort.EventIRQ();
}

void SysTick_Handler(void)
{
	HAL_IncTick();
	ModbusPort.TimerIRQ();
}

} // exten "C"
