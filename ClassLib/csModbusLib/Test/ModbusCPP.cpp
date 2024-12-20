#include "Interface/MbInterface.h"
#include "Interface/MbRTU.h"
#include "Interface/MbASCII.h"
#include "Interface/MbEthMaster.h"
#include "Interface/MbEthSlave.h"

#include "Modbus/MbMaster.h"
#include "Modbus/MbRtuSlaveStm.h"
#include "Modbus/MbAsciiSlaveStm.h"
#include "Modbus/MbSlaveServer.h"
#include "SlaveDataServer/MbSlaveStdServer.h"
#include "SerialWin32.h"

#include "stdafx.h"
#include "conio.h"

using namespace csModbusLib;

#define ASCII
#define SLAVE
#define SLAVE_STM

const char* ComPort = "COM2";
const int BdRate = 19200;
SerialPort* WinPort = new SerialWin32(ComPort, BdRate);

#ifdef RTU
#define IFC_STRING "RTU %s %d",ComPort, BdRate
MbRTU MyInterface = MbRTU(new SerialWin32(ComPort, BdRate));
#endif

#ifdef ASCII
#define IFC_STRING "ASCII %s %d",ComPort, BdRate
MbASCII MyInterface = MbASCII(WinPort);
#endif

#ifdef TCP
#define IFC_STRING "TCP"
#ifdef MASTER
MbTCPMaster MyInterface = MbTCPMaster("127.0.0.1", 502);
#endif
#ifdef SLAVE
MbTCPSlave MyInterface = MbTCPSlave(502);
#endif
#endif

#ifdef UDP
#define IFC_STRING "UDP"
#ifdef MASTER
MbUDPMaster MyInterface = MbUDPMaster("127.0.0.1", 502);

#endif
#ifdef SLAVE
MbUDPSlave MyInterface = MbUDPSlave(502);
#endif
#endif

MbMaster MyMaster = MbMaster(&MyInterface);


#ifdef SLAVE_STM
#ifdef ASCII
MbAsciiSlaveStm MySlave = MbAsciiSlaveStm (WinPort);
#endif
#ifdef RTU
MbRtuSlaveStm MySlave = MbRtuSlaveStm (WinPort);
#endif
#else
MbSlaveServer MySlave = MbSlaveServer(&MyInterface);
#endif

#define NUM_REGS 8
#define BASE_ADDR 10
uint16_t ModbusRegs[NUM_REGS] = { 0 };

uint16_t holding20[5] = { 0 };
coil_t coils10[20] = { 0 };
coil_t inputs20[20] = { 0 };

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
#ifdef SLAVE_STM
	printf("Using SlaveSTM\r\n");
#endif
	printf(IFC_STRING);
	printf("\r\n");

	StdDataServer MyDataServer = StdDataServer(1);

	// abgeleitete Daten Klasse
	MyHoldingRegs.AddData(BASE_ADDR, ModbusRegs, NUM_REGS);
	MyDataServer.AddHoldingRegisters(&MyHoldingRegs);


	MyDataServer.AddHoldingRegisters(20, holding20, 5);
	MyDataServer.AddCoils(10, coils10, 20);
	MyDataServer.AddDiscreteInputs(30, inputs20, 20);

	MySlave.StartListen(&MyDataServer);

	while (_kbhit() == 0) {

		printf("RdCount %d Regdata %04d,%04d,%04d\r", ReadCount, ModbusRegs[0], ModbusRegs[1], ModbusRegs[2]);
		for (int i = 0; i < 20; ++i) {
			inputs20[i] = coils10[i];
		}
		ModbusRegs[7] += 1;
		if (ModbusRegs[6] == 42)
			break;
		MbSleep(50);
	}
	MySlave.StopListen();
}

int errCount = 0;

bool CheckError(ErrorCodes errCode)
{
	if (errCode != ErrorCodes::MB_NO_ERROR) {
		++errCount;
		printf("Error:%d, count:%d\r", errCode, errCount);
		return true;
	}
	return false;
}
void TestMaster()
{
	printf("CPP Modbus Master Test\r\n");
	printf(IFC_STRING);
	printf("\r\n");
	uint16_t tstCount = 0;

	MyMaster.Connect(&MyInterface, 1);
	while (_kbhit() == 0) {
		ErrorCodes retCode = MyMaster.ReadHoldingRegisters(BASE_ADDR, 3, ModbusRegs, 0);
		if (CheckError (retCode) == false) {
			printf("RdRegister %04d,%04d,%04d\r", ModbusRegs[0], ModbusRegs[1], ModbusRegs[2]);
		}
		CheckError(MyMaster.WriteSingleRegister(12, tstCount++));

		coil_t Coils[24];
		CheckError (MyMaster.ReadDiscreteInputs(30, 20, Coils));
		CheckError(MyMaster.WriteMultipleCoils(10, 20, Coils));
		MbSleep(100);

	}
	MyMaster.Close();
}


int _tmain(int argc, _TCHAR* argv[])
{
#ifdef MASTER
	TestMaster();
#endif
#ifdef SLAVE
	TestSlave();
#endif
	return 0;
}

