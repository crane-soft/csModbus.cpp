#include "Modbus/MbSlaveStateMachine.h"

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
		Frame.RawData->Clear();
		ReceiveFrameData(enRxStates::ReceiveHeader, 2, timeout);
	}

	void MbSlaveStateMachine::ErrorOccured(ErrorCodes errCode)
	{
		LastError = errCode;
		ErrorHandler((int)LastError);
		WaitForFrameStart();
	}

	int MbSlaveStateMachine::SerialInterface_DataReceivedEvent(int result)
	{
		int DataLen;

		if (result == -1) {
			ErrorOccured(ErrorCodes::CONNECTION_ERROR);
			return 1;
		}

		if (RxState == enRxStates::AsciiStartOfFrame) {
			AsciiCheckStartFrame(result);

		} else {
			if (result == 0) {
				ErrorOccured(ErrorCodes::RX_TIMEOUT);
				return 1;
			}

			if (AsciiHexData())
				return 1;

			switch (RxState) {
			case enRxStates::ReceiveHeader:
				DataLen = Frame.ParseMasterRequest();
				if (DataLen < 0) {
					ErrorOccured(ErrorCodes::ILLEGAL_FUNCTION_CODE);
				} else {
					ReceiveFrameData(enRxStates::RcvMessage, DataLen);
				}
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
				break;
			default:
				break;
			}
		}
		return 1;
	}

	void MbSlaveStateMachine::MasterRequestReceived()
	{
		if (SerialInterface->Check_EndOfFrame() == false) {
			ErrorOccured(ErrorCodes::WRONG_CRC);
		} else {
			if (DataServices()) {
				SendResponseMessage();
			}
			WaitForFrameStart();
		}
	}
}
