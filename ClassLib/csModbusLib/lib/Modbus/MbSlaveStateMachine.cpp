#include "Modbus/MbSlaveStateMachine.h"
#include "Interface/MbASCII.h"

#include <condition_variable>
#include <chrono>
#include <functional>
#include <thread>

namespace csModbusLib {

	void MbSlaveStateMachine::StartListener()
	{
		SerialInterface = (MbSerial*)gInterface;
		RxState = enRxStates::Idle;
		auto callBackFunc = std::bind(&MbSlaveStateMachine::SerialInterface_DataReceivedEvent, this, std::placeholders::_1);
		SerialInterface->setCallback(callBackFunc);
		WaitForFrameStart();
	}

	void MbSlaveStateMachine::StopListener()
	{
	}


	void MbSlaveStateMachine::ReveiveHeader(int timeout)
	{
		Frame.RawData.Clear();
		ReceiveFrameData(enRxStates::ReceiveHeader, 2, timeout);
	}


	int MbSlaveStateMachine::SerialInterface_DataReceivedEvent(int result)
	{
		int DataLen;
		if (result == -1) {
			ErrorOcurred(ErrorCodes::CONNECTION_ERROR);
		}

		if (RxState == enRxStates::AsciiStartOfFrame) {
			AsciiCheckStartFrame(result);

		} else {
			if (result == 0) {
				ErrorOcurred(ErrorCodes::RX_TIMEOUT);
			}

			if (AsciiHexData())
				return 1;

			switch (RxState) {
			case enRxStates::ReceiveHeader:
				DataLen = Frame.ParseMasterRequest();
				ReceiveFrameData(enRxStates::RcvMessage, DataLen);
				break;
			case enRxStates::RcvMessage:
				DataLen = Frame.ParseDataCount();
				if (DataLen != 0) {
					ReceiveFrameData(enRxStates::RcvAdditionalData, DataLen);
				} else {
					ReceiveFrameEnd();
				}
				break;
			case enRxStates::RcvAdditionalData:
				ReceiveFrameEnd();
				break;
			case enRxStates::RcvCrLf:
				AsciiReceiveCrLf();
				break;
			case enRxStates::EndOfFrame:
				MasterRequestReceived();
			default:
				break;
			}
		}
		return 1;
	}

	void MbSlaveStateMachine::MasterRequestReceived()
	{
		try {
			SerialInterface->Check_EndOfFrame();
			DataServices();
			SendResponseMessage();
		}
		catch (ErrorCodes errCode) {
			ErrorOcurred (errCode);
		}
		WaitForFrameStart();
	}
}
