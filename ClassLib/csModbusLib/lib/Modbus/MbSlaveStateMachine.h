#pragma once
#include "Modbus/mbslave.h"
#include "Interface/MbSerial.h"
#include "Interface/MbASCII.h"

namespace csModbusLib {
	class MbSlaveStateMachine : public MbSlave
	{
	public:
        MbSlaveStateMachine() : MbSlave() {}
        MbSlaveStateMachine(MbSerial* SerialInterface) : MbSlave(SerialInterface) { }
        MbSlaveStateMachine(MbSerial* SerialInterface, MbSlaveDataServer* DataServer) : MbSlave(SerialInterface, DataServer)  {}

    protected:
        virtual void StartListener() override;
        void StopListener() override;

        enum class enRxStates
        {
            Idle,
            AsciiStartOfFrame,
            ReceiveHeader,
            RcvMessage,
            RcvAdditionalData,
            RcvCrLf,
            EndOfFrame,
        };

        virtual void WaitForFrameStart() = 0;
        virtual void ReceiveFrameEnd() = 0;
        virtual void ReceiveFrameData(enRxStates NextState, int DataLen, int timeout = MbInterface::ByteCountTimeout) = 0;

        virtual void AsciiCheckStartFrame(int result) {}
        virtual int AsciiHexData() { return 0; }
        virtual void AsciiReceiveCrLf() {}

        void ReveiveHeader(int timeout = MbInterface::ByteCountTimeout);
        enRxStates RxState = enRxStates::Idle;

    private:
        int SerialInterface_DataReceivedEvent(int result);
        void MasterRequestReceived();
        MbSerial* SerialInterface;
    };
}
