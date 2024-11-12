#pragma once
#include "Modbus/MbSlaveServer.h"
#include "Interface/MbInterface.h"
namespace csModbusLib
{
	void MbSlaveServer::StartListener()
	{
		if (ListenThread == 0) {
			ListenThread = new std::thread(&MbSlaveServer::HandleRequestMessages, this);
		}

	}

	void MbSlaveServer::StopListener()
	{
		if (ListenThread != NULL) {
			while (stopped == false) {
				MbSleep(1);
			}
			ListenThread->join();
			ListenThread = NULL;
		}
	}
}
