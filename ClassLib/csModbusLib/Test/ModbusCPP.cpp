#include "MbMaster.h"
#include "MbSlaveStdServer.h"
#include "MbSlave.h"
#include "MbInterface.h"
#include "MbRTU.h"
#include "MbASCII.h"

#include "stdafx.h"
#include "conio.h"

using namespace csModbusLib;

MbRTU MyInterface = MbRTU("COM9", 19200);
//MbASCII MyInterface = MbASCII("COM9", 19200);
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

	/* EventClasse übergen
	ModbusRegsData *HoldinRegs = new ModbusRegsData(BASE_ADDR, ModbusRegs, NUM_REGS);
	HoldinRegs->RegisterEvents(&ModbusEvent);
	*/

	// abgeleitete Daten Klasse
	MyHoldingRegs.AddData(BASE_ADDR, ModbusRegs, NUM_REGS);

	MyDataServer.AddHoldingRegisters(&MyHoldingRegs);
	MySlave.StartListen(&MyInterface, &MyDataServer);
	while (_kbhit() == 0) {
		printf("RdCount %d Regdata %04d,%04d,%04d\r", ReadCount, ModbusRegs[0], ModbusRegs[1], ModbusRegs[2]);
		ModbusRegs[7] += 1;
		if (ModbusRegs[6] == 42)
			break;
		MbSleep(100);
	}
}


void TestMaster()
{
	printf("CPP Modbus Master Test\r\n");
	uint16_t tstCount = 0;
	MyMaster.Connect(&MyInterface, 1);
	while (_kbhit() == 0) {
		MyMaster.ReadHoldingRegisters(BASE_ADDR, 3, ModbusRegs, 0);
		printf("RdRegister %04d,%04d,%04d\r", ModbusRegs[0], ModbusRegs[1], ModbusRegs[2]);
		MyMaster.WriteSingleRegister(12, tstCount++);
	}
	MyMaster.Close();
}

bool palindrome(char arr[], int size)
{
	for (int i = 0; i < size; ++i) {
		if (arr[i] != arr[size-1-i])
			return false;
	}
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	palindrome("ABBA",4);

	TestSlave();
	return 0;
}

