#pragma once
#include "MbSlave.h"
#include "MbSlaveDataServer.h"
#include <thread>

namespace csModbusLib 
{
	class MbSlaveServer : public MbSlave {
	public:
		MbSlaveServer() : MbSlave() {}
		MbSlaveServer(MbInterface *Interface) : MbSlave(Interface) {}
		MbSlaveServer(MbInterface *Interface, MbSlaveDataServer *DataServer) : MbSlave(Interface, DataServer) {}
	protected:
		void StartListener();
		void StopListener();

	private:
		std::thread *ListenThread = NULL;
	};
}