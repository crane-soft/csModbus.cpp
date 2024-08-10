#include "MbBase.h"
#include "MbFrame.h"
#include "MbSlaveDataServer.h"
#include "MbSlaveServer.h"
#include "platform.h"

namespace csModbusLib 
{
	void MbSlaveServer::StartListener()
	{
		if (ListenThread == 0) {
			ListenThread = new std::thread(&MbSlave::HandleRequestMessages, this);
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
