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

	void MbSlaveServer::HandleRequestMessages()
	{
		running = true;
		stopped = false;
		while (running) {
			try {
				ReceiveMasterRequestMessage();
				if (DataServices()) {
					SendResponseMessage();
				}
			}
			catch (ErrorCodes errCode) {
				if (running) {
					ErrorHandler(errCode);
				}
			}
		}
		stopped = true;
	}

	void MbSlaveServer::ReceiveMasterRequestMessage()
	{
		gInterface->ReceiveHeader(MbInterface::InfiniteTimeout);
		ErrorCodes errCode = Frame.ReceiveMasterRequest(gInterface);
		if (errCode != ErrorCodes::MB_NO_ERROR)
			throw errCode;
	}

}
