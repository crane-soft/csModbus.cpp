#pragma once
#include "Modbus/MbSlave.h"
#include "SlaveDataServer/MbSlaveDataServer.h"
#include <thread>
#include "platform.h"

namespace csModbusLib 
{
	class MbSlaveServer : public MbSlave {
	public:
		MbSlaveServer() : MbSlave() {}
		MbSlaveServer(MbInterface *Interface) : MbSlave(Interface) {}
		MbSlaveServer(MbInterface *Interface, MbSlaveDataServer *DataServer) : MbSlave(Interface, DataServer) {}
		void HandleRequestMessages();

	protected:
		void StartListener();
		void StopListener();
	private:
		void ReceiveMasterRequestMessage();
		std::thread *ListenThread = NULL;
	};
}
