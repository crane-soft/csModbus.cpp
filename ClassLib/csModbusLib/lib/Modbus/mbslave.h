#include "MbInterface.h"
#include "MbBase.h"
#include "MbFrame.h"
#include "MbSlaveDataServer.h"
#include "Stopwatch.h"

#include <thread>

namespace csModbusLib {

    class MbSlave : public MbBase {

	protected:
		MbSlaveDataServer *gDataServer;
	private:
		MBSFrame Frame = MBSFrame();
      
	public:
		MbSlave();
		MbSlave(MbInterface *Interface);
		MbSlave(MbInterface *Interface, MbSlaveDataServer *DataServer);

		MbSlaveDataServer * Get_DataServer();
		void Set_DataServer(MbSlaveDataServer *DataServer);
		void HandleRequestMessages();

	private:
		bool ReceiveMasterRequestMessage();
		void SendResponseMessage();
		void DataServices();
	};

	class MbSlaveServer : public MbSlave {
	private:
		std::thread *ListenThread;
	public:
		MbSlaveServer() : MbSlave() {}
		MbSlaveServer(MbInterface *Interface) : MbSlave(Interface) {}
		MbSlaveServer(MbInterface *Interface, MbSlaveDataServer *DataServer) : MbSlave(Interface, DataServer) {}
		void StartListen();
		void StartListen(MbSlaveDataServer *DataServer);
		void StartListen(MbInterface *Interface, MbSlaveDataServer *DataServer);
		void StopListen();
	};
}
