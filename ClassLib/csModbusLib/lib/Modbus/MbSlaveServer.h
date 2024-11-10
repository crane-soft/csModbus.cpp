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

	protected:
		void StartListener()
		{
			if (ListenThread == 0) {
				ListenThread = new std::thread(&MbSlave::HandleRequestMessages, this);
			}

		}

		void StopListener()
		{
			if (ListenThread != NULL) {
				while (stopped == false) {
					MbSleep(1);
				}
				ListenThread->join();
				ListenThread = NULL;
			}
		}


	private:
		std::thread *ListenThread = NULL;
	};
}
