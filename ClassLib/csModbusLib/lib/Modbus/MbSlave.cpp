#include "Modbus/MbSlave.h"
#include "Interface/MbInterface.h"
#include "platform.h"

namespace csModbusLib {

	MbSlave::MbSlave() {}

	MbSlave::MbSlave(MbInterface *Interface)
	{
		gInterface = Interface;
	}

	MbSlave::MbSlave(MbInterface *Interface, MbSlaveDataServer *DataServer)
	{
		gInterface = Interface;
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

	void MbSlave::SendResponseMessage()
	{
		int MsgLen = Frame.ToMasterResponseMessageLength();
		if (MsgLen > 0)
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
		Frame.RawData = gInterface->getFrameData();
		errCount = 0;
		if (gInterface != 0) {
			if (running) {
				StopListen();
			}

			if (gInterface->Connect()) {
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
		gInterface = Interface;
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

	void MbSlave::HandleRequestMessages()
	{
		running = true;
		stopped = false;
		while (running) {
			TRY {
				ReceiveMasterRequestMessage();
				if (DataServices()) {
					SendResponseMessage();
				}
			}
			CATCH (exCode) {
				if (running) {
					ErrorHandler(exCode);
				}
			}
		}
		stopped = true;
	}

	void MbSlave::ReceiveMasterRequestMessage()
	{
		gInterface->ReceiveHeader(MbInterface::InfiniteTimeout);
		ErrorCodes errCode = Frame.ReceiveMasterRequest(gInterface);
		if (errCode != ErrorCodes::MB_NO_ERROR)
			THROW(errCode);
	}

	void MbSlave::ErrorHandler(int errCode)
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
