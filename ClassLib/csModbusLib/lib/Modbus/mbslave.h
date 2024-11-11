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

	protected:
		MbSlaveDataServer *gDataServer = 0;
		bool stopped = false;
		MBSFrame Frame;// = MBSFrame();
		virtual void StartListener() {}
		virtual void StopListener() {}
		bool DataServices();
		void SendResponseMessage();
		void ErrorHandler(ErrorCodes errCode);
	private:
		ErrorCodes lastErrorCode;
		int errCount;
	};
}
