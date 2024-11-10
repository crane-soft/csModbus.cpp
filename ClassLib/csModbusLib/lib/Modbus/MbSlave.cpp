#include "Modbus/MbSlave.h"
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
				if (DataServices()) {
					SendResponseMessage();
				}
			}
			catch (ErrorCodes errCode) {
				if (running) {
					ErrorOcurred(errCode);
				}
			}
		}
		stopped = true;
	}

	void MbSlave::ReceiveMasterRequestMessage()
	{
			gInterface->ReceiveHeader(MbInterface::InfiniteTimeout);
			Frame.ReceiveMasterRequest(gInterface);
	}

	void MbSlave::SendResponseMessage()
	{
		int MsgLen = Frame.ToMasterResponseMessageLength();
		gInterface->SendFrame(MsgLen);
	}

	bool MbSlave::DataServices()
	{
		MbSlaveDataServer *DataServer = gDataServer;
		bool ID_matched = false;

		while (DataServer != 0) {
			ID_matched |= DataServer->DataServices(&Frame);
			DataServer = DataServer->NextDataServer;
		}
		return ID_matched;
	}

	bool MbSlave::StartListen()
	{
		errCount = 0;
		if (gInterface != 0) {
			if (running) {
				StopListen();
			}

			if (gInterface->Connect(&Frame.RawData)) {
				StartListener();
				return true;
			}
		}
		return false;
	}

	bool MbSlave::StartListen(MbSlaveDataServer *DataServer)
	{
		gDataServer = DataServer;
		return StartListen();
	}

	bool MbSlave::StartListen(MbInterface *Interface, MbSlaveDataServer *DataServer)
	{
		InitInterface(Interface);
		gDataServer = DataServer;
		return StartListen();
	}

	void MbSlave::StopListen()
	{
		stopped = false;
		running = false;
		if (gInterface != 0) {
			gInterface->DisConnect();
		}
		StopListener();
	}

	void MbSlave::ErrorOcurred(ErrorCodes errCode)
	{
		lastErrorCode = errCode;

		// If in case of a communication error, the slave does NOT return a response.
		// The master program eventually processes a timeout condition for the request.
		DebugPrint("\r\nModbusException  %d\r\n", errCode);

		if (++errCount == 10) {
			//running = false;
		}
		gInterface->DiscardReceived();
	}
}
