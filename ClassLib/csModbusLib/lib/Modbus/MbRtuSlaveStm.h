#pragma once
#include "Modbus/MbSlaveStateMachine.h"
#include "Interface/MbRtu.h"

namespace csModbusLib {
	class MbRtuSlaveStm : public MbSlaveStateMachine
	{
	public:
        MbRtuSlaveStm(SerialPort* sp) : MbSlaveStateMachine(&RtuInterface)    {
            RtuInterface.setSerialPort(sp);
        }

        MbRtuSlaveStm(SerialPort* sp, MbSlaveDataServer* DataServer) : MbSlaveStateMachine(&RtuInterface, DataServer) {
            RtuInterface.setSerialPort(sp);
        }

    protected:
        void WaitForFrameStart() override
        {
            ReveiveHeader(MbInterface::InfiniteTimeout);
        }

        void ReceiveFrameData(enRxStates NextState, int DataLen, int timeout = MbInterface::ByteCountTimeout)  override
        {
            RxState = NextState;
            RtuInterface.ReceiveBytesEv(DataLen, timeout);

        }
        void ReceiveFrameEnd()  override
        {
            ReceiveFrameData(enRxStates::EndOfFrame, 2);

        }
    private:
        MbRTU RtuInterface;
    };
}
