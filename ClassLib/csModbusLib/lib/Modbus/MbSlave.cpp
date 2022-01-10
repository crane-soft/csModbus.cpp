﻿#include "MbInterface.h"
#include "MbBase.h"
#include "MbFrame.h"
#include "MbSlaveDataServer.h"
#include "Stopwatch.h"
#include "MbSlave.h"
#include "ThreadUtils.h"


namespace csModbusLib {

	MbSlave::MbSlave()
	{
	}

	MbSlave::MbSlave(MbInterface *Interface)
	{
		gInterface = Interface;
		gDataServer = NULL;
		ListenThread = NULL;
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
	
	void MbSlave::StartListen()
	{
		if (gInterface != 0) {
			if (running) {
				StopListen();
				MbSleep(100);
			}

			if (ListenThread == 0) {
				ListenThread = new std::thread(&MbSlave::HandleRequestMessages,this);
			}
		}
	}

	void MbSlave::StartListen(MbSlaveDataServer *DataServer)
	{
		gDataServer = DataServer;
		StartListen();
	}

	void MbSlave::StartListen(MbInterface *Interface, MbSlaveDataServer *DataServer)
	{
		gInterface = Interface;
		gDataServer = DataServer;
		StartListen();
	}

	void MbSlave::StopListen()
	{
		running = false;
		if (gInterface != 0) {
			gInterface->DisConnect();
		}
		if (ListenThread != NULL) {
			ListenThread->join();
			ListenThread = NULL;
		}

	}

	void MbSlave::HandleRequestMessages()
	{
		running = true;
		gInterface->Connect();
		while (running) {
			try {
				if (ReceiveMasterRequestMessage()) {
					DataServices();
					SendResponseMessage();
					MbSleep(1);
				}
			}
			catch (ErrorCodes errCode) {
				//Console.WriteLine("ModbusException  {0}", errCode);
				std::printf("ModbusException  %d", errCode);
				gInterface->ReConnect();
			}
			//Console.WriteLine("");
		}
	}

	bool MbSlave::ReceiveMasterRequestMessage()
	{
		while (running) {
			if (gInterface->ReceiveHeader(&Frame.RawData)) {
				Frame.ReceiveMasterRequest(gInterface);
				return true;
			}
			MbSleep(1);
		}
		return false;
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
}