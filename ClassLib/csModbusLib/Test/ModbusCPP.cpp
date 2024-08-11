#include "MbMaster.h"
#include "MbSlaveStdServer.h"
#include "MbSlaveServer.h"
#include "MbInterface.h"
#include "MbRTU.h"
#include "MbASCII.h"
#include "MbEthMaster.h"
#include "MbEthSlave.h"

#include "stdafx.h"
#include "conio.h"

using namespace csModbusLib;


MbMaster MyMaster;
MbSlaveServer MySlave;

#define NUM_REGS 8
#define BASE_ADDR 10
uint16_t ModbusRegs[NUM_REGS] = { 0 };
int ReadCount;
class ModbusEventClass : public ModbusDataEvents {
public:
	void ValueChangedEvent(int Addr, int Size) {
		printf ("\r\nValueChanged Addr %d, Size %d\r\n", Addr, Size);
	}
	void ValueReadEvent(int Addr, int Size) {
		++ReadCount;
	}
} ModbusEvent;

class MyModbusRegsData : public ModbusRegsData {
protected:
	void ValueChangedEvent(int Addr, int Size)
	{
		printf("\r\nValueChanged Addr %d, Size %d\r\n", Addr, Size);
	}
	void ValueReadEvent(int Addr, int Size)
	{
		++ReadCount;
	}
} MyHoldingRegs;

void TestSlave()
{
	printf("CPP Modbus Slave Test\r\n");
	StdDataServer MyDataServer = StdDataServer(1);

	/* EventClasse übergeben
	ModbusRegsData *HoldinRegs = new ModbusRegsData(BASE_ADDR, ModbusRegs, NUM_REGS);
	HoldinRegs->RegisterEvents(&ModbusEvent);
	*/

	// abgeleitete Daten Klasse
	MyHoldingRegs.AddData(BASE_ADDR, ModbusRegs, NUM_REGS);

	MyDataServer.AddHoldingRegisters(&MyHoldingRegs);

	uint16_t holding20[5] = {0};
	MyDataServer.AddHoldingRegisters(20, holding20, 5);
	coil_t coils10[20] = { 0 };
	MyDataServer.AddCoils(10, coils10, 20);
	coil_t inputs20[20] = { 0 };
	MyDataServer.AddDiscreteInputs(20, inputs20, 20);

	MbTCPSlave MyInterface = MbTCPSlave(502);

	MySlave.StartListen(&MyInterface, &MyDataServer);
	while (_kbhit() == 0) {
		printf("RdCount %d Regdata %04d,%04d,%04d\r", ReadCount, ModbusRegs[0], ModbusRegs[1], ModbusRegs[2]);
		for (int i = 0; i < 20; ++i) {
			inputs20[i] = coils10[i];
		}
		ModbusRegs[7] += 1;
		if (ModbusRegs[6] == 42)
			break;
		MbSleep(100);
	}
	MySlave.StopListen();
}


void TestMaster()
{
	printf("CPP Modbus Master Test\r\n");
	uint16_t tstCount = 0;
	int errCount = 0;

	MbTCPMaster MyInterface = MbTCPMaster("127.0.0.1", 502);
	MyMaster.Connect(&MyInterface, 1);
	while (_kbhit() == 0) {
		int retCode = MyMaster.ReadHoldingRegisters(BASE_ADDR, 3, ModbusRegs, 0);
		if (retCode == ErrorCodes::MB_NO_ERROR) {
			printf("RdRegister %04d,%04d,%04d\r", ModbusRegs[0], ModbusRegs[1], ModbusRegs[2]);
		} else {
			++errCount;
			printf("Error:%d, count:%d\r", retCode,errCount);
		}
		MyMaster.WriteSingleRegister(12, tstCount++);
		MbSleep(100);

	}
	MyMaster.Close();
}


int _tmain(int argc, _TCHAR* argv[])
{
	//TestMaster();
	TestSlave();
	return 0;
}

