#include "MbInterface.h"
#include "MbBase.h"
#include "MbFrame.h"
#include "MbSlaveDataServer.h"
#include "Stopwatch.h"

#include <thread>

namespace csModbusLib {

    class MbSlave : MbBase {

	private: 
		MbSlaveDataServer *gDataServer;
        MBSFrame Frame = MBSFrame();
		std::thread *ListenThread;
        
	public:
		MbSlave();
		MbSlave(MbInterface *Interface);
		MbSlave(MbInterface *Interface, MbSlaveDataServer *DataServer);

		MbSlaveDataServer * Get_DataServer();
		void Set_DataServer(MbSlaveDataServer *DataServer);

		void StartListen();
		void StartListen(MbSlaveDataServer *DataServer);
		void StartListen(MbInterface *Interface, MbSlaveDataServer *DataServer);
		void StopListen();

	protected:
		void HandleRequestMessages();

	private:
		bool ReceiveMasterRequestMessage();
		void SendResponseMessage();
		void DataServices();
	};
}
