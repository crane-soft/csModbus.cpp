
#include "Modbus/MbRtuSlaveStm.h"
#include "Modbus/MbAsciiSlaveStm.h"
#include "SlaveDataServer/MbSlaveEmbServer.h"
#include "SerialSTM32.h"
#include "Gpio.h"

#define SLAVE_ID	1
using namespace csModbusLib;

#define BAUD_RATE	115200
#define MB_SERVER
//#define STM_RTU
//#define STM_ASCII

SerialSTM32				ModbusPort 		 = SerialSTM32(USART2,BAUD_RATE);

#ifdef MB_SERVER
MbRTU				ModbusInterfave  = MbRTU(&ModbusPort);
MbSlave				ModbusSlave 	 = MbSlave (&ModbusInterfave);
#else
#ifdef STM_RTU
MbRtuSlaveStm		ModbusSlave 	 = MbRtuSlaveStm (&ModbusPort);
#else
MbAsciiSlaveStm		ModbusSlave 	 = MbAsciiSlaveStm (&ModbusPort);
#endif
#endif

#define HOLDING_REGS_ADDR	10
#define MAX_HOLDING_REGS	8
#define INPUS_REGS_ADDR		20
#define MAX_INPUS_REGS		5
uint16_t mod_registers[MAX_HOLDING_REGS];

class Nucleo_DataServer : public MbSlaveEmbServer
{
public:	
	Nucleo_DataServer(int SlaveID)
		: MbSlaveEmbServer(SlaveID) {}
	
protected:

    bool ReadInputRegisters() override
    {
		if (Frame->MatchAddress (INPUS_REGS_ADDR, MAX_INPUS_REGS)) {
			Frame->PutValue(0,HAL_GetTick());
			Frame->PutValue(1,ModbusSlave.GetLastError());
			Frame->PutValue(2,ModbusSlave.GetErrorCount());
			Frame->PutValue(3,0);
			Frame->PutValue(4,0);

			return true;
		}
		return false;
    }

    bool ReadHoldingRegisters() override
	{ 
		if (Frame->PutValues (HOLDING_REGS_ADDR, MAX_HOLDING_REGS, mod_registers)) {
			++mod_registers[0];
			//mod_registers[3] = ModbusSlave.SerialInterface->TimeOutOffs;
			return true; 
		}
		return false;
	}

	bool WriteSingleRegister() override
	{ 
		if (Frame->MatchAddress (HOLDING_REGS_ADDR, MAX_HOLDING_REGS)) {
			mod_registers[Frame->DataAddress - HOLDING_REGS_ADDR] = Frame->GetSingleUInt16();
			++mod_registers[1];
			//ModbusSlave.SerialInterface->TimeOutOffs = mod_registers[3];
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
