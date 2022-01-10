#include "MbMaster.h"
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

MbSlave MySlave;

void TestSlave()
{
	MySlave.StartListen();
}


void TestMaster()
{
	printf("CPP Modbus Master Test\r\n");
	uint16_t Data[10] = {0};
	uint16_t tstCount = 0;
	MyMaster.Connect(&MyInterface, 1);
	while (_kbhit() == 0) {
		MyMaster.ReadHoldingRegisters(10, 3, Data, 0);
		printf("RdRegister %04d,%04d,%04d\r", Data[0], Data[1], Data[2]);
		MyMaster.WriteSingleRegister(12, tstCount++);
	}
	MyMaster.Close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	TestMaster();
	return 0;
}

