#pragma once
#include "Modbus/MbBase.h"
#include "SlaveDataServer/MbSlaveDataServer.h"
#include "platform.h"

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

		void HandleRequestMessages();

		MbSlaveDataServer * Get_DataServer();
		void Set_DataServer(MbSlaveDataServer *DataServer);
		int GetLastError() { return (int)lastErrorCode; }
		int GetErrorCount() { return errCount; }

	protected:
		MbSlaveDataServer *gDataServer = 0;
		bool stopped = false;
		MBSFrame Frame;// = MBSFrame();
		virtual void StartListener() {}
		virtual void StopListener() {}
		bool DataServices();
		void SendResponseMessage();
		void ErrorHandler(int errCode);
	private:
		void ReceiveMasterRequestMessage();
		int lastErrorCode;
		int errCount;
	};
}
