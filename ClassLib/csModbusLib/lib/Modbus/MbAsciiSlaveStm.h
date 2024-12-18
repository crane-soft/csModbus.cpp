#pragma once
#include "Modbus/MbSlaveStateMachine.h"
#include "Interface/MbAscii.h"

namespace csModbusLib {
	class MbAsciiSlaveStm : public MbSlaveStateMachine
	{
	public:
        MbAsciiSlaveStm(SerialPort* sp) : MbSlaveStateMachine(&AsciiInterface)  {
            AsciiInterface.setSerialPort(sp);
        }

        MbAsciiSlaveStm(SerialPort* sp, MbSlaveDataServer* DataServer) : MbSlaveStateMachine(&AsciiInterface, DataServer)  {
            AsciiInterface.setSerialPort(sp);
        }

    protected:
        void WaitForFrameStart() override
        {
            AsciiDataCount = 0;
            RxState = enRxStates::AsciiStartOfFrame;
            AsciiReceiveStart();
        }

        void ReceiveFrameData(enRxStates NextState, int DataLen, int timeout = MbInterface::ByteCountTimeout)  override
        {
            RxState = NextState;
            AsciiDataCount = DataLen;
            AsciiInterface.ReceiveBytesEv(2, timeout);
        }

        void ReceiveFrameEnd()  override
        {
            ReceiveFrameData(enRxStates::RcvCrLf, 1);
        }

        void AsciiCheckStartFrame(int result) override
        {
            if ((result > 0) && (Frame.RawData->LastByte() == MbASCII::ASCII_START_FRAME)) {
                ReveiveHeader();
            } else {
                AsciiReceiveStart();
            }
        }

        int AsciiHexData() override
        {
            if (AsciiDataCount > 0) {
                AsciiInterface.ASCII2Hex();
                if (--AsciiDataCount > 0) {
                    AsciiInterface.ReceiveBytesEv(2);
                    return 1;
                }
            }
            return 0;
        }

        void AsciiReceiveCrLf() override
        {
            RxState = enRxStates::EndOfFrame;
            AsciiDataCount = 0;
            AsciiInterface.ReceiveBytesEv(2);
        }


    private:
        void AsciiReceiveStart()
        {
            Frame.RawData->Clear();
            AsciiInterface.ReceiveBytesEv(1, 10);
        }

        int AsciiDataCount;
        MbASCII AsciiInterface;
    };
}
