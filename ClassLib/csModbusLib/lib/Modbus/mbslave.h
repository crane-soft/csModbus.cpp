#pragma once
#include "MbFrame.h"
#include "MbSlaveDataServer.h"
#include <thread>

namespace csModbusLib {

    class MbSlave : public MbBase {
      
	public:
		MbSlave();
		MbSlave(MbInterface *Interface);
		MbSlave(MbInterface *Interface, MbSlaveDataServer *DataServer);

		MbSlaveDataServer * Get_DataServer();
		void Set_DataServer(MbSlaveDataServer *DataServer);
		void HandleRequestMessages();

	protected:
		MbSlaveDataServer *gDataServer = NULL;
		bool stopped = false;

	private:
		MBSFrame Frame = MBSFrame();
		void ReceiveMasterRequestMessage();
		void SendResponseMessage();
		void DataServices();
	};

	class MbSlaveServer : public MbSlave {
	public:
		MbSlaveServer() : MbSlave() {}
		MbSlaveServer(MbInterface *Interface) : MbSlave(Interface) {}
		MbSlaveServer(MbInterface *Interface, MbSlaveDataServer *DataServer) : MbSlave(Interface, DataServer) {}
		bool StartListen();
		bool StartListen(MbSlaveDataServer *DataServer);
		bool StartListen(MbInterface *Interface, MbSlaveDataServer *DataServer);
		void StopListen();
	private:
		std::thread *ListenThread = NULL;
	};
}
