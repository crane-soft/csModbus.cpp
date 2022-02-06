#include "MbBase.h"
#include "MbFrame.h"
#include "MbSlaveDataServer.h"
#include "MbSlave.h"
#include "platform.h"

namespace csModbusLib {

	MbSlave::MbSlave() {}

	MbSlave::MbSlave(MbInterface *Interface)
	{
		InitInterface(Interface);
	}

	MbSlave::MbSlave(MbInterface *Interface, MbSlaveDataServer *DataServer)
	{
		InitInterface(Interface);
		gDataServer = DataServer;
	}

	MbSlaveDataServer * MbSlave::Get_DataServer()
	{
		return gDataServer;
	}

	void MbSlave::Set_DataServer(MbSlaveDataServer *DataServer)
	{
		gDataServer = DataServer;
	}
	
	void MbSlave::HandleRequestMessages()
	{
		running = true;
		stopped = false;
		while (running) {
			try {
				ReceiveMasterRequestMessage();
				DataServices();
				SendResponseMessage();
			}
			catch (ErrorCodes errCode) {
				if (running) {
					DebugPrint("ModbusException  %d", errCode);
					gInterface->ReConnect();
				}
			}
		}
		stopped = true;
	}

	void MbSlave::ReceiveMasterRequestMessage()
	{
			gInterface->ReceiveHeader(MbInterface::InfiniteTimeout, &Frame.RawData);
			Frame.ReceiveMasterRequest(gInterface);
	}

	void MbSlave::SendResponseMessage()
	{
		int MsgLen = Frame.ToMasterResponseMessageLength();
		gInterface->SendFrame(&Frame.RawData, MsgLen);
	}

	void MbSlave::DataServices()
	{
		MbSlaveDataServer *DataServer = gDataServer;
		while (DataServer != 0) {
			DataServer->DataServices(&Frame);
			DataServer = DataServer->NextDataServer;
		}
	}

	bool MbSlaveServer::StartListen()
	{
		if (gInterface != 0) {
			if (running) {
				StopListen();
			}

			if (gInterface->Connect()) {
				if (ListenThread == 0) {
					ListenThread = new std::thread(&MbSlave::HandleRequestMessages, this);
				}
				return true;
			}
		}
		return false;
	}

	bool MbSlaveServer::StartListen(MbSlaveDataServer *DataServer)
	{
		gDataServer = DataServer;
		return StartListen();
	}

	bool MbSlaveServer::StartListen(MbInterface *Interface, MbSlaveDataServer *DataServer)
	{
		InitInterface(Interface);
		gDataServer = DataServer;
		return StartListen();
	}

	void MbSlaveServer::StopListen()
	{
		stopped = false;
		running = false;
		if (gInterface != 0) {
			gInterface->DisConnect();
		}
		if (ListenThread != NULL) {
			while ( stopped == false) {
				MbSleep(1);
			}
			ListenThread->join();
			ListenThread = NULL;
		}
	}
}
