#pragma once
#include "Modbus/MbBase.h"
#include "SlaveDataServer/MbSlaveDataServer.h"

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
		bool DataServices();
		void SendResponseMessage();
		void ErrorOcurred(ErrorCodes errCode);
	private:
		void ReceiveMasterRequestMessage();
		ErrorCodes lastErrorCode;
		int errCount;
	};
}
