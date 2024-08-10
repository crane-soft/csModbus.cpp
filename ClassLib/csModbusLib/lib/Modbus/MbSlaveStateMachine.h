#pragma once
#include "mbslave.h"
#include "MbSerial.h"

namespace csModbusLib {
	class MbSlaveStateMachine : public MbSlave
	{
	public:
        MbSlaveStateMachine() : MbSlave() {}
        MbSlaveStateMachine(MbInterface* Interface) : MbSlave(Interface) {}
        MbSlaveStateMachine(MbInterface* Interface, MbSlaveDataServer* DataServer) : MbSlave(Interface, DataServer) {}

    protected:
        void StartListener();
        void StopListener();
    private:
        enum class enRxStates
        {
            Idle,
            StartOfFrame,
            ReceiveHeader,
            RcvMessage,
            RcvAdditionalData,
            RcvEndOfFrame
        };
        void WaitForFrameStart();
        void WaitFrameEnd();
        void WaitFrameData(enRxStates NextState, int DataLen);
        void InitReceiveDataEvent(enRxStates NextState);
        void InitTimeoutTimer();
        void SetTimeOut(int timeOut);
        void TimeoutTimer_Elapsed();
        void SerialInterface_DataReceivedEvent();
        void MasterRequestReceived();

        enRxStates RxState = enRxStates::Idle;
        MbSerial *SerialInterface;
        SerialPort *sp;
        //System.Timers.Timer TimeoutTimer;
        int DataBytesNeeded;
        int serialBytesNeeded;
	};
}
