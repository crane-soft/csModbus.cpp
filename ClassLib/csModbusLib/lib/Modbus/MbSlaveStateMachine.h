#pragma once
#include "Modbus/mbslave.h"
#include "Interface/MbSerial.h"

namespace csModbusLib {
	class MbSlaveStateMachine : public MbSlave , protected SerialCallBack
	{
	public:
        MbSlaveStateMachine() : MbSlave() {}
        MbSlaveStateMachine(MbInterface* Interface) : MbSlave(Interface) {}
        MbSlaveStateMachine(MbInterface* Interface, MbSlaveDataServer* DataServer) : MbSlave(Interface, DataServer) {}
		void CheckStatus();

    protected:
        void StartListener();
        void StopListener();

        // Overides SerialCallBack.DataReceived
        virtual void DataReceived(int bytesAvailavle);

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
