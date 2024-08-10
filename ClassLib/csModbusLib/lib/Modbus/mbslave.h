#pragma once
#include "MbBase.h"
#include "MbSlaveDataServer.h"

namespace csModbusLib {

    class MbSlave : public MbBase {
      
	public:
		MbSlave();
		MbSlave(MbInterface *Interface);
		MbSlave(MbInterface *Interface, MbSlaveDataServer *DataServer);

		bool StartListen();
		bool StartListen(MbSlaveDataServer* DataServer);
		bool StartListen(MbInterface* Interface, MbSlaveDataServer* DataServer);
		void StopListen();

		MbSlaveDataServer * Get_DataServer();
		void Set_DataServer(MbSlaveDataServer *DataServer);
		void HandleRequestMessages();

	protected:
		MbSlaveDataServer *gDataServer = 0;
		bool stopped = false;
		MBSFrame Frame = MBSFrame();
		virtual void StartListener() {}
		virtual void StopListener() {}

	private:
		void ReceiveMasterRequestMessage();
		void SendResponseMessage();
		void DataServices();
	};
}
